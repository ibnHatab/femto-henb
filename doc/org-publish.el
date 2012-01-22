
(setq org-publish-project-alist
      '(("orgfiles"
	 :base-directory "."
	 :base-extension "org"
;;	 :publishing-directory "/ssh:user@host:~/html/notebook/"
	 :publishing-directory "doc"

	 :publishing-function org-publish-org-to-html
	 :exclude "PrivatePage.org"   ;; regexp
	 :headline-levels 3
	 :section-numbers nil
	 :table-of-contents nil
	 :style "<link rel=\"stylesheet\"
                       href=\"../other/mystyle.css\" type=\"text/css\"/>"
	 :html-preamble t)

	;; ("images"
	;;  :base-directory "~/images/"
	;;  :base-extension "jpg\\|gif\\|png"
	;;  :publishing-directory "/ssh:user@host:~/html/images/"
	;;  :publishing-function org-publish-attachment)

	;; ("other"
	;;  :base-directory "~/other/"
	;;  :base-extension "css\\|el"
	;;  :publishing-directory "/ssh:user@host:~/html/other/"
	;;  :publishing-function org-publish-attachment)
	("website" :components ("orgfiles" "images" "other"))))