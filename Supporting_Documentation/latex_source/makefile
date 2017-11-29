NAME=submission
PDF=$(NAME).pdf

all:
	pdflatex $(NAME)
	bibtex $(NAME)
	pdflatex $(NAME)
	pdflatex $(NAME)
	cp $(PDF) ..

update:
	pdflatex $(NAME)
	cp $(PDF) ..

clean:
	rm -vf *.toc *.aux *.dvi *.bbl *.blg *.log *.out *.fls *fdb_latexmk *.synctex.gz  *~ $(PDF)
