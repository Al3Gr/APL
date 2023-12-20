from pymongo import MongoClient, errors


class MongoDB():

    def __init__(self, username, pwd):
        self.client = MongoClient(
            "mongodb://localhost:9080/", username=username, password=pwd)
        self.db = self.client["apl"]
        self.collection = self.db["tags"]

    def addTags(self, tags):
        pass
