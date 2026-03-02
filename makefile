# ----------------------------
# Makefile Options
# ----------------------------

NAME = TIMUS
DESCRIPTION = "TiMus App. A messanger APP for the calculator"
COMPRESSED = YES
COMPRESSED_MODE = zx0 #Default: zx7; zx0 is better tho
ARCHIVED = YES
#ICON = icon.png
CFLAGS = -Wall -Wextra -Oz #C
CXXFLAGS = -Wall -Wextra -Oz #C++
OUTPUT_MAP = YES #Default yes. To speed up compile, jsut turn it off
LTO = YES #Default yes. Decreases Compile Size
PREFER_OS_LIBC = YES #Copies Data from flash (RTC)
PREFER_OS_CRT = NO #Copies Data from flash (RTC)
#MAKE_GFX = cd $(GFXDIR) && $(CONVIMG) #To make Graphics
# ----------------------------

include $(shell cedev-config --makefile)

