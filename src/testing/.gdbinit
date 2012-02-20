set history save on 
set logging on 
target remote localhost:1234 
# load modules
add-symbol-file ../modules/lte_enb.ko 0xbf01c000
add-symbol-file test.ko 0xbf020000
