.pragma library

// From com.nokia.meego.ToolIcon
function handleIconSource(iconId) {
    var prefix = "icon-m-"
    // check if id starts with prefix and use it as is
    // otherwise append prefix and use the inverted version if required
    if (iconId.indexOf(prefix) !== 0)
        iconId =  prefix.concat(iconId); // .concat(theme.inverted ? "-white" : "");
    return "image://theme/" + iconId;
}
