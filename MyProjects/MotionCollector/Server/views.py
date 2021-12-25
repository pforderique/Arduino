from flask import Blueprint, request
from .models import ForceSensorReading, Visitor
from datetime import datetime
from . import db
import requests

views = Blueprint('views', __name__)

@views.route("/", methods=["GET"])
def root():
    return "Welcome to Data Collector !!"

@views.route("/hello/Piero", methods=["GET"])
def hello():
    return "Hello!!"

@views.route("/database", methods=["POST"])
def poster():
    user = request.form.get("user")
    sensor_reading = request.form.get("sensor_reading")

    # add the visitor and commit em
    visitor = Visitor(visitor_name=user, date_visited=datetime.now())
    db.session.add(visitor)
    db.session.commit()    

    # data processing goes here
    url = "google.com/force-sensor"
    requests.post(url, {"sensor_reading": sensor_reading})
    
    return f'user {user} has posted to database! !!!!!'

@views.route("/force", methods=["POST"])
def post_force():
    sensor_number = request.form.get("sensor_number")
    sensor_value = request.form.get("sensor_value")

    # store to database real quick
    data = ForceSensorReading(
        sensor_number=sensor_number,
        sensor_value=sensor_value,
        date_sent=datetime.now()
        )
    db.session.add(data)
    db.session.commit()

    # addtional data processing here
    # ****

    return f"force sensor #{sensor_number} has provided value {sensor_value}"



# my_project/
    # Server/
        # database.db
    # app.py -- runner file