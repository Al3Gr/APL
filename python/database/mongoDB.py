from pymongo import MongoClient, errors


class MongoDB():

    def __init__(self, hostname, port, username, password, database, collection):
        self.__client = MongoClient("mongodb://"+hostname+":"+port, username=username, password=password, connect=False)
        try:
            _ = self.__client.admin.command("ismaster")
            self.__db = self.__client[database]
            self.__coll = self.__db[collection]
        except errors.ServerSelectionTimeoutError as se:
            raise se


    def addTags(self, tags):
        for t in tags:
            doc = self.__coll.find_one_and_update(
                {"tag": t}, {"$inc": {"inference": 1}})
            if (not doc):
                self.__coll.insert_one({"tag": t, "inference": 1})

    def getTags(self):
        raw_tags = self.__coll.find()
        tags = {}
        for t in raw_tags:
            tags.update({t["tag"]:t["inference"]})
        return tags
