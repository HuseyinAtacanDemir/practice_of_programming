import yfinance as yf
import argparse
from datetime import datetime
from typing import List


def fetch_stock_data(ticker: str, period: str) -> dict:
    """
    Fetches stock data for a given ticker and period.
    
    Args:
        ticker (str): The stock ticker symbol.
        period (str): The period for which to retrieve data (e.g., '1d', '5d', '1mo').
    
    Returns:
        dict: A dictionary containing stock data (last close, open, high, low, volume, change).
    """
    stock = yf.Ticker(ticker)
    data = stock.history(period=period)
    
    if data.empty:
        raise ValueError(f"No data found for ticker {ticker} with period {period}.")
    
    # Use iloc for position-based indexing
    last_close = data['Close'].iloc[-1]
    last_open = data['Open'].iloc[-1]
    last_high = data['High'].iloc[-1]
    last_low = data['Low'].iloc[-1]
    volume = data['Volume'].iloc[-1]
    
    # Calculate the change in price
    change = round(last_close - last_open, 4)
    
    # Return the data as a dictionary
    return {
        "ticker": ticker,
        "last_close": last_close,
        "last_open": last_open,
        "last_high": last_high,
        "last_low": last_low,
        "volume": volume,
        "change": change
    }


def format_stock_data(data: dict) -> str:
    """
    Formats the stock data as a string in the required format.
    
    Args:
        data (dict): The stock data dictionary.
    
    Returns:
        str: The formatted stock data string.
    """
    current_time = datetime.now().strftime("%I:%M%p")
    date_today = datetime.now().strftime("%m/%d/%Y")
    change_direction = "+" if data['change'] >= 0 else ""
    
    return (f'"{data["ticker"]}",{data["last_close"]},"{date_today}","{current_time}",'
            f'{change_direction}{data["change"]},{data["last_open"]},{data["last_high"]},'
            f'{data["last_low"]},{data["volume"]}')


def get_stock_data(tickers: List[str], period: str) -> None:
    """
    Retrieves and prints stock data for a list of tickers.
    
    Args:
        tickers (List[str]): A list of stock ticker symbols.
        period (str): The period for which to retrieve data.
    
    Prints:
        Stock data in the specified format.
    """
    for ticker in tickers:
        try:
            data = fetch_stock_data(ticker, period)
            print(format_stock_data(data))
        except ValueError as e:
            print(e)


def parse_arguments() -> argparse.Namespace:
    """
    Parses command-line arguments for the script.
    
    Returns:
        argparse.Namespace: Parsed command-line arguments.
    """
    parser = argparse.ArgumentParser(description="Fetch stock data for multiple tickers.")
    
    # Add argument for stock tickers
    parser.add_argument('-t', '--tickers', nargs='+', required=True,
                        help="List of stock ticker symbols (e.g., MSFT AAPL GOOGL).")
    
    # Add argument for period
    parser.add_argument('-p', '--period', default='1d',
                        help="Period for stock data (default: 1d). Examples: 5d, 1mo, etc.")
    
    return parser.parse_args()


if __name__ == "__main__":
    # Parse the command-line arguments
    args = parse_arguments()
    
    # Fetch and print stock data for the provided tickers
    get_stock_data(args.tickers, args.period)

