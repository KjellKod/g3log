if( NOT DEFINED BPC_NISOM_BUILD_PREFIX )
	set( BPC_NISOM_BUILD_PREFIX "${CMAKE_SOURCE_DIR}-build/NISOM" )
endif()

function( create_nisom_build_dir config )
	set( ARGS
		"${CMAKE_SOURCE_DIR}"
		"-DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/CMakeToolchainNISOM.cmake"
		"-DCMAKE_BUILD_TYPE=${config}" 
		"-B${BPC_NISOM_BUILD_PREFIX}/${config}"
		"-GNMake Makefiles JOM"
	)

	message( STATUS "" )
	message( STATUS "Executing cmake -G\"NMake Makefiles JOM\" -DCMAKE_BUILD_TYPE=${config}" )
	execute_process(
		COMMAND ${CMAKE_COMMAND} "${CMAKE_SOURCE_DIR}" ${ARGS}
	)
	
	message( STATUS "Executing jom install in ${BPC_NISOM_BUILD_PREFIX}/${config}" )
	execute_process(
		COMMAND "jom" "install"
		WORKING_DIRECTORY "${BPC_NISOM_BUILD_PREFIX}/${config}"
	)
endfunction()

if ( NOT DEFINED CONFIG )
	create_nisom_build_dir( Debug )
	create_nisom_build_dir( RelWithDebInfo )
else()
	create_nisom_build_dir( ${CONFIG} )
endif()
