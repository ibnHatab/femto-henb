set history save on 
set logging on 
target remote localhost:1234 
# load modules
add-symbol-file lte_enb.ko 0xbf068000
