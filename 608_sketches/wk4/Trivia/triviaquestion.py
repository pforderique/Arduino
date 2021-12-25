#################################
# perform GET request for the trivia API
#################################
import requests

TRIVIA_API = "http://opentdb.com/api.php?amount=1&type=boolean"

def request_handler(request):
    r = requests.get(TRIVIA_API)
    json = r.json()

    # print(json)
    QUESTION = str(json["results"][0]["question"])
    DIFFICULTY = str(json["results"][0]["difficulty"])
    CORRECT_ANSWER = str(json["results"][0]["correct_answer"]) 

    QUESTION = QUESTION.replace("&quot;", '"')
    QUESTION = QUESTION.replace("&#039;", "'")

    # print(QUESTION)
    # print(DIFFICULTY)
    # print(CORRECT_ANSWER)

    res = QUESTION + ":" + DIFFICULTY + ":" + CORRECT_ANSWER
    print(res)
    return res


if __name__ == "__main__":
    request_handler({"values":{"num":1} })