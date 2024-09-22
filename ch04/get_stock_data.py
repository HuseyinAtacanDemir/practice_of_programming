import yfinance as yf
import argparse
from datetime import datetime
from typing import List


def fetch_stock_data(ticker: str, period: str, interval: str = None) -> List[dict]:
    """
    Fetches stock data for a given ticker, period, and optional interval.
    
    Args:
        ticker (str): The stock ticker symbol.
        period (str): The period for which to retrieve data (e.g., '1mo', '6mo').
        interval (str): The interval for the stock data (e.g., '1d' for daily). 
                        If None, fetches only the latest data for the period.
    
    Returns:
        List[dict]: A list of dictionaries containing stock data.
                    If no interval is given, returns only the last available data.
    """
    stock = yf.Ticker(ticker)

    # If interval is provided, fetch historical data for each day
    if interval:
        data = stock.history(period=period, interval=interval)
    else:
        data = stock.history(period=period)

    if data.empty:
        raise ValueError(f"No data found for ticker {ticker} with period {period}.")

    stock_data = []
    
    if interval:
        # If interval is provided, get data for each row (i.e., each day)
        for date, row in data.iterrows():
            stock_data.append({
                "ticker": ticker,
                "date": date.strftime("%Y-%m-%d"),
                "last_close": row['Close'],
                "last_open": row['Open'],
                "last_high": row['High'],
                "last_low": row['Low'],
                "volume": row['Volume'],
                "change": round(row['Close'] - row['Open'], 4)
            })
    else:
        # If no interval is given, return just the latest available data
        last_row = data.iloc[-1]
        stock_data.append({
            "ticker": ticker,
            "date": datetime.now().strftime("%Y-%m-%d"),
            "last_close": last_row['Close'],
            "last_open": last_row['Open'],
            "last_high": last_row['High'],
            "last_low": last_row['Low'],
            "volume": last_row['Volume'],
            "change": round(last_row['Close'] - last_row['Open'], 4)
        })

    return stock_data


def format_stock_data(data: dict) -> str:
    """
    Formats the stock data as a string in CSV format, including the ticker.
    
    Args:
        data (dict): The stock data dictionary.
    
    Returns:
        str: The formatted stock data string in CSV format.
    """
    return (f'{data["ticker"]}, {data["date"]}, {data["last_close"]},'
            f'{data["last_open"]}, {data["last_high"]}, {data["last_low"]}, {data["volume"]},'
            f'{data["change"]}')


def print_csv_header() -> None:
    """
    Prints the CSV header to describe each field.
    """
    header = "ticker, date, last_close, last_open, last_high, last_low, volume, change"
    print(header)


def get_stock_data(tickers: List[str], period: str, interval: str = None) -> None:
    """
    Retrieves and prints stock data for a list of tickers in CSV format with a header.
    
    Args:
        tickers (List[str]): A list of stock ticker symbols.
        period (str): The period for which to retrieve data.
        interval (str): The interval for the stock data (e.g., daily, hourly). If None, behaves as before.
    
    Prints:
        Stock data in CSV format with a header.
    """
    print_csv_header()  # Print the CSV header before the data
    for ticker in tickers:
        try:
            data_list = fetch_stock_data(ticker, period, interval)
            for data in data_list:
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
    parser.add_argument('-p', '--period', default='1mo',
                        help="Period for stock data (default: 1mo). Examples: 1d, 5d, 1mo, etc.")
    
    # Add argument for interval (optional)
    parser.add_argument('-i', '--interval', default=None,
                        help="Interval for stock data (e.g., 1d, 1h, etc.). If not provided, behaves as before.")
    
    return parser.parse_args()


if __name__ == "__main__":
    # Parse the command-line arguments
    args = parse_arguments()
    
    # Fetch and print stock data for the provided tickers
    get_stock_data(args.tickers, args.period, args.interval)

