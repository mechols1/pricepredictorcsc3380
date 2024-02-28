import tensorflow as tf
from tensorflow.keras.preprocessing.sequence import TimeseriesGenerator
import numpy as np
import sys
import pandas as pd
from get_path import *
import os
import plotly.graph_objects as go

"""
create_model(n):

This function creates a Sequential Model using tensorflow 
The model has fully connected layers with relu activation


n : input days

one output : $
"""
def create_model(n):
    model = tf.keras.Sequential()
    model.add(tf.keras.layers.Dense(64, activation='relu', input_shape=n))
    model.add(tf.keras.layers.Dense(64, activation='relu'))
    model.add(tf.keras.layers.Dense(1))
    return model



"""
This function will help us decide how many inputs the network should have. 
This function takes the number of inputs to check the number of epochs to train for
This function will create a network, prepare data for it, then train the netowrk and evalueate its performance on the test set
"""
def generate_models(df, start, end, epochs):
    # Use input data from day t to predict targets on day t + 1
    inputs = df.drop(columns=['date'])
    targets = df['close']
    models = {}

    for num_inputs in range(start, end + 1):
        generator = TimeseriesGenerator(inputs.values, targets.values, length=num_inputs, batch_size=len(targets))
        #print(f'Using {num_inputs} inputs')
        model = create_model((num_inputs, inputs.shape[1]))
        #print('Training')
        model.compile(optimizer='adam', loss='mse')
        #print("Number of batches:", len(generator))
        h = model.fit(generator, epochs=epochs, verbose = False)
        model_info = {'model': model, 'history': h.history}
        models[num_inputs] = model_info
    return models


def get_best_model(models, og_df):
    model_stats = {}
    for k, v in models.items():
        train_history = v['history']
        loss = train_history['loss'][-1]
   
        model_stats[k] = {'inputs': k, 'loss': loss}
    
    optimal_inputs = min(model_stats, key=lambda k: model_stats[k]['loss'])
    h_min = og_df['close'].min()
    h_max = og_df['close'].max()
    for k in model_stats:
        if k == optimal_inputs:
            e = (h_max - h_min) * model_stats[k]['loss'] + h_min
            normal_e = model_stats[k]['loss']
            print(f'True Error for best model (ie: {k} inputs) = {e}')
            print(f'Normalized Error for best model (ie: {k} inputs) = {normal_e}')

    best_model = models[optimal_inputs]['model']
    return best_model, optimal_inputs


"""
Function to get next weekday

* Won't need it until we real-time predictions ar 


def get_next_weekday(date_string):
    try:
        date_obj = datetime.strptime(date_string, "%Y-%m-%d")
        days_to_add = 1
        while True:
            next_date = date_obj + timedelta(days=days_to_add)
            if next_date.weekday() < 5:  # Monday to Friday (0 to 4)
                return next_date.strftime("%Y-%m-%d")
            days_to_add += 1
    except ValueError:
        return "Invalid date format"
"""

def predict_next_day(model, inputs, input_df, og_df):
    h_min = og_df['close'].min()
    h_max = og_df['close'].max()
    val_inputs = input_df.drop(columns=['date']).iloc[-inputs:].values
    next_day_prediction = model.predict(np.array([val_inputs]))
    predicted_close = next_day_prediction[0][0] * (h_max - h_min) + h_min
    print("="*50)
    return (predicted_close[0], og_df['close'].iloc[-1])


def main_function(processed_df, df, start=1, end=3, epochs=10, user_path=get_path()):

    models = generate_models(processed_df, start=start, end=end, epochs=epochs)
    
    best_model, num_inputs = get_best_model(models, df)
    generator = TimeseriesGenerator(processed_df.drop(columns=['date']).values, processed_df['close'].values, length=num_inputs, batch_size=len(processed_df))
    i, t = generator[0]
    val_predictions = best_model.predict([i])
    temp = val_predictions
    val_predictions = []
    h_max = df['close'].max()
    h_min = df['close'].min()
    val_predictions = [i[0] * (h_max - h_min) + h_min for i in temp]
    val_predictions = [i[0] for i in val_predictions]
    old_df = df
    df = df.iloc[:len(val_predictions)]
    
    trace1 = go.Scatter(
        x=df['date'],
        y=df['close'],
        mode='lines+markers',
        name='True Price',
        line=dict(color='purple', width=4)  # Reduced width for the true price line
    )

    trace2 = go.Scatter(
        x=df['date'],
        y=np.array(val_predictions),
        mode='lines',
        name='Predicted Price',
        # Increased width and changed color for the predicted price line
        line=dict(color='yellow', width=2)
    )

    layout = go.Layout(
        title='True vs Predicted Prices',
        xaxis=dict(title='Date'),
        yaxis=dict(title='Close Price'),
        legend=dict(x=0, y=1, bgcolor='rgba(255,255,255,0)')
    )

    fig = go.Figure(data=[trace1, trace2], layout=layout)

    fig.update_layout(
        title='True Vs. Predicted Prices',
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

    html = fig.to_html(full_html=False, include_plotlyjs='cdn')
    file_path = os.path.join(
        f'{user_path}csc3380-fall-2023-project-group-3/data/', 'true_vs_pred.html')
    with open(file_path, 'w') as file:
        file.write(html)

    predicted, true = predict_next_day(best_model, num_inputs, processed_df, old_df)
    return (predicted, true)

def main():
    args = sys.argv[1:]

    # Expecting at least four arguments: ticker, start, end, epochs
    if len(args) < 4:
        print("Usage:")
        print("<script_name> <ticker> <start> <end> <epochs>")
        sys.exit(1)

    # Assigning each argument to a variable
    ticker, start, end, epochs = args[0], int(args[1]), int(args[2]), int(args[3])

    user_path = get_path()
    df = pd.read_csv(f'{user_path}csc3380-fall-2023-project-group-3/data/processed_{ticker}.csv')
    og_df = pd.read_csv(f'{user_path}csc3380-fall-2023-project-group-3/data/{ticker}.csv')

    predicted, true = main_function(df, og_df, start, end, epochs, user_path)
    if predicted != 0 or true != 0:
        print(f"Predictions for {str(og_df['date'].iloc[-1])[:10]}:\nPredicted price = ${predicted:.2f}\nTrue price = ${true:.2f}")

if __name__ == '__main__':
    main()