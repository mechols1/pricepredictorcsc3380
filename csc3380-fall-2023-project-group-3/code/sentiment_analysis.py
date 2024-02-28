import numpy as np
from nltk.sentiment.vader import SentimentIntensityAnalyzer as sia
from datetime import datetime, timedelta
import requests
import nltk
nltk.download('vader_lexicon')


URL = "https://data.alpaca.markets/v1beta1/news"

# Set the API key and secret
API_KEYS = {
    "APCA-API-KEY-ID": "PKHL949M71P5B43DNVKX",
    "APCA-API-SECRET-KEY": "tI2AoMbdQD7XjCEMIVsRrDi8gVW6vTYWkku8MsT2"
}


def get_news(ticker, start_date, end_date):

    start_date = datetime.fromisoformat(
        start_date).strftime("%Y-%m-%dT%H:%M:%SZ")
    end_date = datetime.fromisoformat(end_date).strftime("%Y-%m-%dT%H:%M:%SZ")

    params = {
        "symbols": ticker,
        "start": start_date,
        "end": end_date,
    }

    response = requests.get(URL, headers=API_KEYS, params=params)

    news_data = response.json()
    try:
        return news_data['news']
    except KeyError:
        return 0

def get_sentiments(df):

    # Extract dates from stock data
    dates_in_stock_data = [datetime.strptime(
        date, "%Y-%m-%d").date() for date in df['date']]

    sentiments = []
    analyzer = sia()

    for i, current_date in enumerate(dates_in_stock_data):
        ticker = "TSLA"
        day_beginning = current_date

        # Check if this is the last date in stock_data, if not then get the next trading day
        next_trading_day = dates_in_stock_data[i +
                                            1] if i+1 < len(dates_in_stock_data) else None

        # If this isn't the last date and there's a gap in trading days, combine sentiment scores
        if next_trading_day and (next_trading_day - current_date).days > 1:
            day_end = next_trading_day
        else:
            day_end = current_date + timedelta(days=1)

        tsla_news = get_news(ticker, start_date=str(
            day_beginning), end_date=str(day_end))

        if tsla_news != 0:
            articles = [article['headline'] for article in tsla_news]
            scores = [analyzer.polarity_scores(
                article)['compound'] for article in articles]
            sentiments.append(np.mean(scores))
        else:
            sentiments.append(0)  # neutral sentiment for no news

    return sentiments
