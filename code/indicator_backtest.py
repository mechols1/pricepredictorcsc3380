import pandas as pd
import os
import plotly.graph_objects as go
import warnings
warnings.filterwarnings('ignore')


class IndicatorBacktest:
    def __init__(self, csv_filename, initial_capital, position_size):
        self.df = pd.read_csv(csv_filename)
        self.initial_capital = initial_capital
        self.position_size = position_size
        self.capital = initial_capital
        self.shares = 0
        self.profits = []

    def reset_state(self):
        self.capital = self.initial_capital
        self.shares = 0
        self.profits = []
   
    def create_base_figure(self):
        fig = go.Figure()
        fig.add_trace(go.Scatter(x=self.df['date'],
                      y=self.df['close'], mode='lines', name='close'))
        return fig

    def get_buy_sell_coordinates(self, signal_column):
        df = self.df
        capital = self.capital
        position_size = self.position_size
        shares = self.shares
        profits = self.profits

        buy_signals_x = []
        buy_signals_y = []
        sell_signals_x = []
        sell_signals_y = []
 
        for i in range(len(df)):
            if df[signal_column].iloc[i] == 1:
                buy_signals_x.append(df['date'].iloc[i])
                buy_signals_y.append(df['close'].iloc[i])
                shares += position_size / df['close'].iloc[i]
            elif df[signal_column].iloc[i] == -1:
                sell_signals_x.append(df['date'].iloc[i])
                sell_signals_y.append(df['close'].iloc[i])
                if shares > 0:
                    profit = shares * df['close'].iloc[i] - position_size
                    capital += profit
                    profits.append(profit)
                    shares = 0

        self.capital = capital
        self.shares = shares
        self.profits = profits

        return buy_signals_x, buy_signals_y, sell_signals_x, sell_signals_y

    def add_buy_sell_signals(self, fig, signal_column):
        buy_signals_x, buy_signals_y, sell_signals_x, sell_signals_y = self.get_buy_sell_coordinates(
            signal_column)

        # Add buy signals to the plot
        fig.add_trace(go.Scatter(
            x=buy_signals_x, y=buy_signals_y,
            mode='markers', marker_symbol='triangle-up',
            marker_line_color='green', marker_color='green',
            marker_size=7, name='Buy Signal'
        ))

        # Add sell signals to the plot
        fig.add_trace(go.Scatter(
            x=sell_signals_x, y=sell_signals_y,
            mode='markers', marker_symbol='triangle-down',
            marker_line_color='red', marker_color='red',
            marker_size=7, name='Sell Signal'
        ))

    def finalize_plot(self, fig, title):
        fig.update_layout(
            title=title,
            yaxis_title='Price',
            xaxis_title='Date',
            legend_title='Legend',
            paper_bgcolor='black',      # Sets the background color of the paper to black
            plot_bgcolor='black',       # Sets the background color of the plot to black
            # Sets the font color to white for contrast
            font=dict(color='white'),
            xaxis=dict(
                rangeslider=dict(
                    visible=False
                ),
                showgrid=False,         # Hides the grid for a cleaner look
                # Sets the x-axis tick labels to white
                tickfont=dict(color='white')
            ),
            yaxis=dict(
                showgrid=False,         # Hides the grid for a cleaner look
                # Sets the y-axis tick labels to white
                tickfont=dict(color='white')
            ),
            legend=dict(
                # Makes the legend background transparent
                bgcolor='rgba(0,0,0,0)',
                # Sets the legend font color to white
                font=dict(color='white')
            )
        )

        #fig.show()
        #return fig.to_html(full_html=False, include_plotlyjs='cdn')

    ##############
    # Indicators #
    def plot_bollinger_bands(self):
        fig = self.create_base_figure()
        # bands
        # upper band
        fig.add_trace(go.Scatter(x=self.df['date'], y=self.df['bollinger bands upper band'], fill=None,
                                 mode='lines', line_color='rgba(0,100,80,0.2)', name='Upper Band'))

        # # middle band
        # fig.add_trace(go.Scatter(x=self.df.index, y=self.df['bollinger bands mid band'], mode='lines', line=dict(
        #     dash='dash'), name='Middle Band'))

        # lower band
        fig.add_trace(go.Scatter(x=self.df['date'], y=self.df['bollinger bands lower band'], fill='tonexty', mode='lines',
                                 fillcolor='rgba(0,100,80,0.2)', line_color='rgba(0,100,80,0.2)', name='Lower Band'))

        self.add_buy_sell_signals(fig, 'bollinger bands signals')
        self.finalize_plot(
            fig, f"Bollinger Bands with Buy/Sell Signals Final Profit = ${format(round(self.capital, 2), ',')}")
        
        
        html = fig.to_html(full_html=False, include_plotlyjs='cdn')
        self.reset_state()
        file_path = os.path.join(
            '/Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3/data/', 'bollinger_bands.html')
        with open(file_path, 'w') as file:
            file.write(html)
        return html


    def plot_dual_thrust(self):
        fig = self.create_base_figure()
        # Add filled region for upper and lower bounds
        fig.add_trace(go.Scatter(
            x=self.df['date'], y=self.df['dual thrust upperbound'], fill=None, mode='lines', line_color='#355c7d', name='Upper Bound'))

        fig.add_trace(go.Scatter(x=self.df['date'], y=self.df['dual thrust lowerbound'], fill='tonexty', mode='lines',
                                 fillcolor='rgba(53, 92, 125, 0.2)', line_color='#355c7d', name='Lower Bound'))

        self.add_buy_sell_signals(fig, 'dual thrust signals')
        self.finalize_plot(
            fig, f"Dual Thrust with Buy/Sell Signals Final Profit = ${format(round(self.capital, 2), ',')}")
        html = fig.to_html(full_html=False, include_plotlyjs='cdn')
        self.reset_state()
        file_path = os.path.join(
            '/Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3/data/', 'dual_thrust.html')
        with open(file_path, 'w') as file:
            file.write(html)
        return html
      
    def plot_heikin_ashi(self):
        fig = self.create_base_figure()

        # First subplot: Heikin-Ashi candlestick
        fig.add_trace(go.Candlestick(x=self.df['date'],
                                     open=self.df['HA open'],
                                     high=self.df['HA high'],
                                     low=self.df['HA low'],
                                     close=self.df['HA close'],
                                     increasing_line_color='red',
                                     decreasing_line_color='green',
                                     name='Heikin-Ashi',
                                     whiskerwidth=.9))

        self.add_buy_sell_signals(fig, 'HA signals')
        self.finalize_plot(
            fig, f"Heikin-Ashi with Buy/Sell Signals Final Profit = ${format(round(self.capital, 2), ',')}")
        html = fig.to_html(full_html=False, include_plotlyjs='cdn')
        self.reset_state()
        file_path = os.path.join(
            '/Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3/data/', 'heikin_ashi.html')
        with open(file_path, 'w') as file:
            file.write(html)
        return html
        
    
    def plot_awesome(self):
        fig = self.create_base_figure()

        fig.add_trace(go.Scatter(
            x=self.df['date'], y=self.df['awesome ma1'], mode='lines', name='Awesome MA1'))
        fig.add_trace(go.Scatter(x=self.df['date'], y=self.df['awesome ma2'], mode='lines',
                                 name='Awesome MA2', line=dict(dash='dot')))

        self.add_buy_sell_signals(fig, 'awesome signals')
        self.finalize_plot(
            fig, f"Awesome Oscillator with Buy/Sell Signals Final Profit = ${format(round(self.capital, 2), ',')}")
        html = fig.to_html(full_html=False, include_plotlyjs='cdn')
        self.reset_state()
        file_path = os.path.join(
            '/Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3/data/', 'awesome.html')
        with open(file_path, 'w') as file:
            file.write(html)
        return html
      

    def plot_macd(self):
        fig = self.create_base_figure()

        fig.add_trace(go.Scatter(
            x=self.df['date'], y=self.df['macd ma1'], mode='lines', name='MACD MA1'))
        fig.add_trace(go.Scatter(x=self.df['date'], y=self.df['macd ma2'], mode='lines',
                                 name='MACD MA2', line=dict(dash='dot')))

        self.add_buy_sell_signals(fig, 'macd signals')
        self.finalize_plot(
            fig, f"MACD with Buy/Sell Signals Final Profit = ${format(round(self.capital, 2), ',')}")
        html = fig.to_html(full_html=False, include_plotlyjs='cdn')
        self.reset_state()
        file_path = os.path.join(
            '/Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3/data/', 'macd.html')
        with open(file_path, 'w') as file:
            file.write(html)

        return html

    def plot_sar(self):
        fig = self.create_base_figure()

        fig.add_trace(go.Scatter(x=self.df['date'], y=self.df['parabolic sar'], mode='lines', name='Parabolic SAR', line=dict(
            dash='dot', color='white')))

        self.add_buy_sell_signals(fig, 'sar signals')
        self.finalize_plot(
            fig, f"Parabolic SAR with Buy/Sell Signals Final Profit = ${format(round(self.capital, 2), ',')}")
        html = fig.to_html(full_html=False, include_plotlyjs='cdn')
        self.reset_state()
        file_path = os.path.join(
            '/Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3/data/', 'parabolic_sar.html')
        with open(file_path, 'w') as file:
            file.write(html)
    
        return html

    def plot_rsi(self):
        fig = self.create_base_figure()

        self.add_buy_sell_signals(fig, 'RSI signals')
        self.finalize_plot(
            fig, f"RSI with Buy/Sell Signals Final Profit = ${format(round(self.capital, 2), ',')}")
        html = fig.to_html(full_html=False, include_plotlyjs='cdn')
        self.reset_state()
        # Save HTML to a file
        file_path = os.path.join('/Users/rileyoest/VS_Code/csc3380-fall-2023-project-group-3/data/', 'rsi.html')
        with open(file_path, 'w') as file:
            file.write(html)
        return html
