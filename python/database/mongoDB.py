from configparser import ConfigParser
from pymongo import MongoClient, errors


class MongoDB():

    def __init__(self):
        config = ConfigParser()
        config.read("/Users/alessandrogravagno/Desktop/Università/APL/Progetto/python/config/config.ini")
        self.hostname = config.get('MongoDB', 'Hostname')
        self.port = config.get('MongoDB', 'Port')
        self.username = config.get('MongoDB', 'Username')
        self.password = config.get('MongoDB', 'Password')
        self.database = config.get('MongoDB', 'Database')
        self.collection = config.get('MongoDB', 'Collection')
        self.client = MongoClient("mongodb://"+self.hostname+":" +
                                  self.port+"/", username=self.username, password=self.password)
        self.db = self.client[self.database]
        self.coll = self.db[self.collection]

    def addTags(self, tags):
        for t in tags:
            doc = self.coll.find_one_and_update(
                {"tag": t}, {"$inc": {"inference": 1}})
            if (not doc):
                self.coll.insert_one({"tag": t, "inference": 1})
