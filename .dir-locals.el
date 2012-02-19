;;; Directory Local Variables
;;; See Info node `(emacs) Directory Variables' for more information.

((nil
  (tab-width . 4)
  (c-basic-offset . 4)
  (c-set-style "linux")
  (compilation-read-command . nil)
  (compile-command . (format "make -C %s %s"
                             (file-name-directory (get-closest-pathname "Makefile"))
                             "qemu-test"))
))