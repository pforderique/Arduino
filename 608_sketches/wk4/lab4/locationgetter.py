import datetime
import sqlite3
mit_db = '__HOME__/lab04b/mit.db'

def bounding_box(point_coord,box):
    max_x = max(box, key=lambda x: x[0])[0]
    min_x = min(box, key=lambda x: x[0])[0]
    max_y = max(box, key=lambda y: y[1])[1]
    min_y = min(box, key=lambda y: y[1])[1]

    x, y = point_coord
    return x <= max_x and x >= min_x and y >= min_y and y <= max_y

def translate(point, origin):
    return (point[0] - origin[0], point[1] - origin[1])

def intersection_point(vertex_1, vertex_2):
    x1, y1 = vertex_1
    x2, y2 = vertex_2
    return (x1 * y2 - y1 * x2) / (y2 - y1)

def within_area(point_coord,poly):
    poly = [(x - point_coord[0], y - point_coord[1]) for x, y in poly]
    crossed_edges = 0

    for i, vertex_1 in enumerate(poly):
        vertex_2 = poly[(i + 1) % len(poly)]
        if sign(vertex_1[1]) != sign(vertex_2[1]):
            if sign(vertex_1[0]) == 1 and sign(vertex_2[0]) == 1:
                crossed_edges += 1
            elif sign(vertex_1[0]) != sign(vertex_2[0]):
                if intersection_point(vertex_1, vertex_2) >= 0:
                    crossed_edges += 1

    return crossed_edges % 2 != 0

locations={
    "Student Center":[(-71.095863,42.357307),(-71.097730,42.359075),(-71.095102,42.360295),(-71.093900,42.359340),(-71.093289,42.358306)],
    "Dorm Row":[(-71.093117,42.358147),(-71.092559,42.357069),(-71.102987,42.353866),(-71.106292,42.353517)],
    "Simmons/Briggs":[(-71.097859,42.359035),(-71.095928,42.357243),(-71.106356,42.353580),(-71.108159,42.354468)],
    "Boston FSILG (West)":[(-71.124664,42.353342),(-71.125737,42.344906),(-71.092478,42.348014),(-71.092607,42.350266)],
    "Boston FSILG (East)":[(-71.092409,42.351392),(-71.090842,42.343589),(-71.080478,42.350900),(-71.081766,42.353771)],
    "Stata/North Court":[(-71.091636,42.361802),(-71.090950,42.360811),(-71.088353,42.361112),(-71.088267,42.362476),(-71.089769,42.362618)],
    "East Campus":[(-71.089426,42.358306),(-71.090885,42.360716),(-71.088310,42.361017),(-71.087130,42.359162)],
    "Vassar Academic Buildings":[(-71.094973,42.360359),(-71.091776,42.361770),(-71.090928,42.360636),(-71.094040,42.359574)],
    "Infinite Corridor/Killian":[(-71.093932,42.359542),(-71.092259,42.357180),(-71.089619,42.358274),(-71.090928,42.360541)],
    "Kendall Square":[(-71.088117,42.364188),(-71.088225,42.361112),(-71.082774,42.362032)],
    "Sloan/Media Lab":[(-71.088203,42.361017),(-71.087044,42.359178),(-71.080071,42.361619),(-71.082796,42.361905)],
    "North Campus":[(-71.11022,42.355325),(-71.101280,42.363934),(-71.089950,42.362666),(-71.108361,42.354484)],
    "Technology Square":[(-71.093610,42.363157),(-71.092130,42.365837),(-71.088182,42.364188),(-71.088267,42.362650)]
}

def sign(x):
    if x > 0:
        return 1
    elif x == 0:
        return 0
    else:
        return -1

def get_area(point_coord,locations):
    for loc_name, loc_coords in locations.items():
        if within_area(point_coord, loc_coords):
            return loc_name

    return "Off Campus"

def create_database():
    conn = sqlite3.connect(mit_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS mit_table (user text, lat real, lon real, location text, timing timestamp);''') # run a CREATE TABLE command
    conn.commit() # commit commands
    conn.close() # close connection to database

def request_handler(request):
        
    create_database()

    if request["method"] == "GET":
        lat = 0
        lon = 0
        try:
            lat = float(request['values']['lat'])
            lon = float(request['values']['lon'])
        except Exception as e:
            # return e here or use your own custom return message for error catch
            #be careful just copy-pasting the try except as it stands since it will catch *all* Exceptions not just ones related to number conversion.
            return "Error: lat, lon are missing or not numbers"

        return get_area([lon,lat],locations)
    else:
        #* POST METHOD ######
        try:
            USER = str(request['form']['user'])
            lat = float(request['form']['lat'])
            lon = float(request['form']['lon'])
            location = get_area([lon,lat],locations)
            timing = datetime.datetime.now()

            #* insert into OUR database
            conn = sqlite3.connect(mit_db)
            c = conn.cursor()
            c.execute('''INSERT into mit_table VALUES (?,?,?,?,?);''',(USER,lat,lon,location,timing))
            conn.commit()
            conn.close()

            #* select from database
            conn = sqlite3.connect(mit_db)
            c = conn.cursor()
            thirty_secs_ago = datetime.datetime.now()- datetime.timedelta(seconds=30) # create time for fifteen minutes ago!
            users = c.execute('''SELECT user FROM mit_table WHERE location = ? AND timing > ?''',(location, thirty_secs_ago)).fetchall()
            conn.commit()
            conn.close()

            # create our response
            response = str(location) + '\n'
            for user in users:
                response += str(user[0]) + '\n'

            return response

        except Exception as e:
            # return e here or use your own custom return message for error catch
            #be careful just copy-pasting the try except as it stands since it will catch *all* Exceptions not just ones related to number conversion.
            return "not all parameters received"