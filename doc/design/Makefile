source = fap3_design.xml

stylesheet=/usr/share/xml/docbook/stylesheet/nwalsh/html/onechunk.xsl

index.html: $(source)
	xsltproc --stringparam toc.section.depth 3 $(stylesheet) $(source)
clean:
	rm index.html
