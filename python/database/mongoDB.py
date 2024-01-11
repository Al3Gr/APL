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
        for key, value in tags.items():
            self.__coll.insert_one({"tag": key, "percentage": value})

    def getTags(self, threshold):
        pipeline = [{"$match": {"percentage" :{"$gt": threshold}}},{"$group": {"_id": "$tag", "count": {"$sum": 1}}}]
        raw_tags = self.__coll.aggregate(pipeline)
        tags = {}
        for t in raw_tags:
            tags[t["_id"]] = t["count"]
        return tags
