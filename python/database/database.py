# database.py
from flask_sqlalchemy import SQLAlchemy

db = SQLAlchemy()

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(80), unique=True, nullable=False)

class Notification(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    error = db.Column(db.Boolean, nullable=False)
    event = db.Column(db.String(80), nullable=False)
    message = db.Column(db.String(200), nullable=False)
    user = db.Column(db.String(80), nullable=False)
    read = db.Column(db.Boolean, default=False)
