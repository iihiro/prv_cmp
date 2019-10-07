# glob all cpp file as fullpath/.cpp to ${SOURCES}
file(GLOB SOURCES
	"*.cpp"
)

set(LINK_LIBS PALISADEpke PALISADEcore ntl)

set(EXECS "")
foreach (SOURCE_CPP ${SOURCES})
	get_filename_component(EXENAME ${SOURCE_CPP} NAME_WE)
	get_filename_component(SOURCENAME ${SOURCE_CPP} NAME)
  add_executable(${EXENAME} ${SOURCENAME})
	target_link_libraries(${EXENAME} ${LINK_LIBS})
	set(EXECS ${EXECS} ${EXENAME}) 
endforeach (SOURCE_CPP)
