;;; Directory Local Variables
;;; See Info node `(emacs) Directory Variables' for more information.

((nil
  (tab-width . 8)
  (c-basic-offset . 4)
  (c-set-style "linux")
  (compilation-read-command . nil)

   ;; (compile-command . (format "make -C %s %s"
   ;;                            (get-closest-pathname "TurnerProject")
   ;;                            "run_cl_testing"))

    (compile-command . (format "make -C %s/LTERT %s"
                               (get-closest-pathname "T4KDev")
                               "-f makefile-larm fapi"))
  ))