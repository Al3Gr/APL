from pymongo import MongoClient, errors


class MongoDB:

    # In fase di creazione dell'istanza, creo la connessione col server sul quale si trova il database con il quale voglio comunicare
    # Passo host e porta su cui è in ascolto, le credenziali di accesso e la collezione da usare
    def __init__(self, hostname, port, username, password, database, collection):
        self.__client = MongoClient("mongodb://"+hostname+":"+port, username=username, password=password, connect=False)
        try:
            _ = self.__client.admin.command("ismaster")
            self.__db = self.__client[database]
            self.__coll = self.__db[collection]
        except errors.ServerSelectionTimeoutError as se:
            # Se non riesco a connetermi al server entro il timeout impostato, sollevo un eccezione
            raise se

    # Aggiungo i tag con relativa percentuale di precisione nel database
    def addTags(self, tags):
        for key, value in tags.items():
            self.__coll.insert_one({"tag": key, "percentage": value})

    # Ottengo i tag dal server
    # In particolare, sulla base della soglia passata, ottengo i tag con precisione maggiore di essa
    # ed aggrego quelli con nome uguale, in modo tale da avere un'informazione su quanti tag ho ottenuto con una precisione
    # maggiore o uguale di quella passata
    def getTags(self, threshold):
        pipeline = [{"$match": {"percentage": {"$gt": threshold}}}, {"$group": {"_id": "$tag", "count": {"$sum": 1}}}]
        raw_tags = self.__coll.aggregate(pipeline)
        tags = {}
        for t in raw_tags:
            tags[t["_id"]] = t["count"]
        return tags
