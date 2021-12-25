#################################
# perform GET request for YAHOO Stock
#################################
import requests
from bs4 import BeautifulSoup

YAHOO_SITE = 'https://finance.yahoo.com/quote/'

def request_handler(request):

    # get ticker from request
    try: ticker = request['values']['ticker']
    except: return "ERROR: No Ticker Provided."

    # get the content from the site and geed to parser
    content = requests.get(YAHOO_SITE + ticker)
    soup = BeautifulSoup(content.text, features="html.parser")

    # get price
    obj = soup.find(attrs={'class': 'Trsdu(0.3s) Fw(b) Fz(36px) Mb(-4px) D(ib)','data-reactid':"50"})
    price = obj.text

    print(price) 
    return price # str

# if __name__ == "__main__":
#     request_handler({"values":{"ticker":"TSLA"} })