
function fileName(fph) {
    var fp = fph.toString();
    if (fp[fp.length -1] == '/')
        return fp;
    var last = fp.lastIndexOf('/');
    if (last < 0)
        return '.';
    return fp.slice(last + 1);
}

function baseName(fph) {
    var fn = fph;
    var index = fn.lastIndexOf("."); //TODO add check for some special files like .tar.gz or .app.zip
    if(index !== -1)
        fn = fn.substring(0, index);
    return fn;
}

function dbusToCppName(fn) {
    fn = baseName(fn); //TODO remove this hack
    fn = fn.replace(/\./g, '_').toLowerCase() + '.cpp';
    return fn;
}
