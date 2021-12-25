#################################
# update 'database' using new score update
#################################

DATABASE_FILEPATH = r'C:\Users\fabri\OneDrive\Documents\Arduino\Class_Sketches\wk4\Trivia\triviadb.txt'
# NOTE: ^this was for testing locally. On server, PATH = "/var/jail/home/porderiq/desex/triviaex/triviadb.txt"

def request_handler(request):
    try:
        n = int(request["values"]["num"]) # num was a str

        # read in previous
        with open(DATABASE_FILEPATH, 'r') as f:
            total = int(f.read())

        total += n
        total = str(total)

        with open(DATABASE_FILEPATH, 'w') as f:
            f.write(total)

        print(total)
        return total # as str

    except: 
        return "NUM REQUIRED"

# if __name__ == "__main__":
#     request_handler({"values":{"num":1} })