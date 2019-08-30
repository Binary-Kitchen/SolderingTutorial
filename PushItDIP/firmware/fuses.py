Import('env')
env.Replace(FUSESCMD="$UPLOADER $UPLOADERFLAGS -Ulock:w:0xFF:m -Uhfuse:w:0xD9:m -Uefuse:w:0xFD:m -Ulfuse:w:0xFF:m")