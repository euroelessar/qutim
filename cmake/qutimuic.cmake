# Copyright (c) 2009, Ruslan Nigmatullin, <euroelessar@gmail.com>


EXECUTE_PROCESS(COMMAND ${QUTIM_UIC_EXECUTABLE}
  -tr qtr
  ${QUTIM_UIC_FILE}
  OUTPUT_VARIABLE _uic_CONTENTS
  ERROR_QUIET
)

set(QUTIM_UIC_CPP_FILE ${QUTIM_UIC_H_FILE})

STRING(REGEX REPLACE "#ifndef " "#ifndef UI_" _uic_CONTENTS "${_uic_CONTENTS}")
STRING(REGEX REPLACE "#define " "#define UI_" _uic_CONTENTS "${_uic_CONTENTS}")

FILE(WRITE ${QUTIM_UIC_CPP_FILE} "#include <qutim/plugininterface.h>\n\n${_uic_CONTENTS}\n")
