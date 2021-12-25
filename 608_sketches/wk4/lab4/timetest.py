# import sqlite3
# import time
# example_db = "example.db" #make sure this is run in the same directory as your other file.
# conn = sqlite3.connect(example_db)
# c = conn.cursor()

# start = time.time()
# things = c.execute('''SELECT * FROM test_table ORDER BY favorite_number DESC;''').fetchall()
# print(things[0])
# print(time.time()-start)

# start = time.time()
# thing = c.execute('''SELECT * FROM test_table ORDER BY favorite_number DESC;''').fetchone()
# print(thing)
# print(time.time()-start)

# conn.commit()
# conn.close()



# import sqlite3
# import datetime
# example_db = "time_example.db" # just come up with name of database
# conn = sqlite3.connect(example_db)  # connect to that database (will create if it doesn't already exist)
# c = conn.cursor()  # move cursor into database (allows us to execute commands)
# c.execute('''CREATE TABLE IF NOT EXISTS dated_table (user text,favorite_number int, timing timestamp);''') # run a CREATE TABLE command
# conn.commit() # commit commands
# conn.close() # close connection to database


# import sqlite3
# import datetime
# import string
# import random
# import time
# example_db = "time_example.db" # name of database from above
# conn = sqlite3.connect(example_db)  # connect to that database
# c = conn.cursor()  # move cursor into database (allows us to execute commands)
# for x in range(5):
#     number = random.randint(0,1000000) # generate random number from 0 to 1000000
#     user = ''.join(random.choice(string.ascii_letters + string.digits) for i in range(10)) # make random user name
#     c.execute('''INSERT into dated_table VALUES (?,?,?);''',(user,number,datetime.datetime.now())) #with time
#     print("inserting {}".format(user)) #notification message
#     time.sleep(2) #pause for 2 seconds before next insert
# conn.commit() # commit commands
# conn.close() # close connection to database



# import sqlite3
# import datetime
# example_db = "time_example.db" # name of database from above
# conn = sqlite3.connect(example_db)  # connect to that database
# c = conn.cursor()  # move cursor into database (allows us to execute commands)
# things = c.execute('''SELECT * FROM dated_table;''').fetchall()
# for x in things:
#     print(x)
# conn.commit() # commit commands
# conn.close() # close connection to database

import sqlite3
import datetime
example_db = "time_example.db" # name of database from above
conn = sqlite3.connect(example_db)  # connect to that database
c = conn.cursor()  # move cursor into database (allows us to execute commands)
fifteen_minutes_ago = datetime.datetime.now()- datetime.timedelta(minutes = 15) # create time for fifteen minutes ago!
things = c.execute('''SELECT * FROM dated_table WHERE timing > ? ORDER BY timing DESC;''',(fifteen_minutes_ago,)).fetchall()
for x in things:
    print(x)
conn.commit() # commit commands
conn.close() # close connection to database