import requests
from bs4 import BeautifulSoup

'''BeautifulSoup Documentation (may prove to be helpful in stripping html <b>, <p>, <u> tags, etc...:
https://www.crummy.com/software/BeautifulSoup/bs4/doc/
'''

def request_handler(request):
    # get the topic
    try: 
        topic = str(request['values']['topic'])
        max_len = int(request['values']['len'])
    except: return '-1'

    #use the string below for properly formatted wikipedia api access (https://www.mediawiki.org/wiki/API:Main_page)
    to_send = "https://en.wikipedia.org/w/api.php?titles={}&action=query&prop=extracts&redirects=1&format=json&exintro=".format(topic)
    r = requests.get(to_send)
    data = r.json()

    # get the 'extract'
    extract = data['query']['pages'] # contains another dictionary of size 1 -- I just want 
    for key in extract.keys():
        # key will be the page number. This changes so we don't know what'll be. However, its '-1' if no results found
        if key == '-1': return '-1'
        else: extract = extract[key]['extract']
        break

    soup = BeautifulSoup(extract, 'html.parser')
    extract = soup.get_text()

    # strip the extract
    extract = extract[:max_len].strip('\n')
    for i in range(-1, -1-len(extract), -1):
        if extract[i] == '.':
            i = -i
            extract = extract[:len(extract) - i + 1]
            break

    return extract

if __name__ == "__main__":
    print(request_handler({'args': ['topic', 'len'], 'values': {'topic': 'cat', 'len': '100'}, 'method': 'GET'}))

    # print(request_handler({'values': {'topic': 'yoda', 'len': 398} }))

            
