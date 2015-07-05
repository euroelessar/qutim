import "../Protocol.qbs" as Protocol

Protocol {
    cpp.defines: base.concat([ "LIBOSCAR_LIBRARY" ])
}
