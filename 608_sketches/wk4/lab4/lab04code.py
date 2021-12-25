import sqlite3
import datetime
#/var/jail/home/porderiq/
visits_db = '__HOME__/lab04b/time_example.db'


def request_handler(request):

    # create the database real quick - if not made already
    conn = sqlite3.connect(visits_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS dated_table (user text,favorite_number int, timing timestamp);''') # run a CREATE TABLE command
    conn.commit() # commit commands
    conn.close() # close connection to database

    

    conn = sqlite3.connect(visits_db)  #connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  #move cursor into database (allows us to execute commands)
    fifteen_minutes_ago = datetime.datetime.now()- datetime.timedelta(minutes = 15) #create time for fifteen minutes ago!
    things = c.execute('''SELECT * FROM dated_table WHERE timing > ? ORDER BY timing DESC;''',(fifteen_minutes_ago,)).fetchall()
    outs = ""
    for x in things:
        outs+=str(x)+"\n"
    conn.commit() #commit commands
    conn.close() #close connection to database
    return outs