import QtQuick 2.1

ListModel {
    id: model

    function getAll(ownerId, count, offset) {
        if (!offset)
            offset = 0;
        if (!count)
            count = 200;

        var args = {
            "owner_id"  : ownerId,
            "offset"    : offset,
            "count"     : count,
            "extended"  : 0
        }
        var reply = client.request("photos.getAll", args)
        reply.resultReady.connect(function(response) {
            var count = response.shift()
            for (var index in response) {
                var photo = response[index]
                model.append(photo)
            }
        })
    }

    function _find(photoId) {
        for (var index = 0; index != model.count; index++)
            if (get(index).pid === photoId)
                return photoId
        return -1
    }
}
