board_runner_args(linkserver "--device=MCXN947:FRDM-MCXN947" "--core=cm33_core0")
board_runner_args(jlink "--device=MCXN947_M33_0" "--reset-after-load")

include(${ZEPHYR_BASE}/boards/common/linkserver.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
