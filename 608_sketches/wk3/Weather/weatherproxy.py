import requests
from datetime import datetime

LAT = '32.5493849'
LNG = '-84.8935396'
API_KEY = '9afe1fb84c78548e8473542b05e07eb6'

WEATHER_API = 'http://api.openweathermap.org/data/2.5/weather?lat={0}&lon={1}&appid={2}'.format(LAT, LNG, API_KEY)
DATETIME_API = 'http://iesc-s3.mit.edu/esp32test/currenttime'

def request_handler(request):
    response = ""
    try:
        num = int(request["values"]["num"]) # num is str
        print('*! request called for num =', num)

        if request["method"] == "POST":
            return "POST requests not allowed."

        # make sure its an option
        if num < 1 or num > 6:
            return "ERROR"

        # if asked for time or date -- request to TIME_API
        if num == 2 or num == 3: 
            r = requests.get(DATETIME_API)

            if num == 2: # time
                date = datetime.strptime(r.text[11:19], "%H:%M:%S")  # 24 hr format
                return date.strftime("%I:%M:%S %p") 
            else: # num == 3 -> date
                return r.text[:10]

        # else request ot the weather API
        else:
            r = requests.get(WEATHER_API)
            json = r.json()

            if num == 1: # tempature K->F
                K = json['main']['temp'] 
                F = 1.8*(K - 273) + 32
                response = str(round(F, 2)) + " F"

            elif num == 4: # weather
                response = str(json['weather'][0]['main'])

            elif num == 5: # humidity
                response = str(json['main']['humidity']) + "%"

            else: # num = 6 -> pressure
                response = str(json['main']['pressure']) + " hPa"
    except:
        return "NUM IS REQUIRED"

    return response

# testing
# for i in range(1,7):
#     print(request_handler(i))