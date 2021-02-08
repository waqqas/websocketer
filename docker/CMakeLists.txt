
configure_file(Dockerfile.in Dockerfile)

add_custom_target(build-docker
  DEPENDS build-docker-command
)


add_custom_command(
  OUTPUT build-docker-command
  VERBATIM
  COMMENT "Buildin docker"
  COMMAND docker build -t ${PROJECT_NAME}_dev ${CMAKE_CURRENT_BINARY_DIR}
)


add_custom_target(run-docker
  DEPENDS run-docker-command
)


add_custom_command(
  OUTPUT run-docker-command
  VERBATIM
  COMMENT "Running docker"
  COMMAND docker run --name ${PROJECT_NAME} ${PROJECT_NAME}_dev
)

