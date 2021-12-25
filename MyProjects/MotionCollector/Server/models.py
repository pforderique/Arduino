from sqlalchemy.sql.expression import table
from . import db

class Visitor(db.Model):
    '''model to keep track of how many people have visited the site'''
    __tablename__ = "visitors"
    visitor_name = db.Column(db.String, primary_key=True, unique=True)
    date_visited = db.Column(db.DateTime, unique=True)

# --------------
# Visitor table
# ---------------
# 0 | visitor_number | date_visited | ...|


class ForceSensorReading(db.Model):
    '''model to keep track of force sensor readings'''
    __tablename__ = "force_sensor"
    _id = db.Column(db.Integer, primary_key=True, unique=True) # might not even 
    sensor_number = db.Column(db.Integer) # 1, 2, ..., 6
    sensor_value = db.Column(db.Integer)
    date_sent = db.Column(db.DateTime, unique=True)

    
# class Gyro(db.Model):