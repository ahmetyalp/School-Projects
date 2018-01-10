#lang scheme
; basic card game in scheme
;compiling: yes
;complete: yes
;2015400213

;4.1-------------------------------------------------------------------------------------
; (card-color one-card) -> string?
; one-card : pair?
;
; Returns color of one-card
(define (card-color one-card)
  (if (list? (member (car one-card) (list 'H 'D)))
      'red 
      'black))

;4.2--------------------------------------------------------------------------
; (card-rank one-card) -> number?
; one-card : pair?
; Returns rank of one-card
(define (card-rank one-card)
  (cond ((equal? (cdr one-card) 'A) 11)
        ((list? (member (cdr one-card) (list 'K 'Q 'J))) 10)
        ((exact-positive-integer? (cdr one-card)) (cdr one-card))))

;4.3------------------------------------------------------------------------
; (all-same-color list) -> boolean?
; list : list?
; Returns #t if all the cards in list-of-cards have same color, #f otherwise
(define (all-same-color list)
  (cond ((null? list) #t)
  		((null? (cdr list)) #t)
        ((equal? (card-color (car list)) (card-color (cadr list))) (all-same-color (cdr list)))
        (else #f)))

;4.4---------------------------------------------------------------------
; (fdraw card-list held-cards) -> list?
; card-list : list?
; held-cards : list?
; Returns a new list of held-cards after the action draw is taken.
(define (fdraw card-list held-cards)
  (cond ((null? card-list) held-cards)
        (else (list* (car card-list) held-cards))))

;4.5---------------------------------------------------------
; (fdiscard list-of-cards list-of-moves goal held-cards) -> list?
; list-of-cards : list?
; list-of-moves : list?
; goal : number?
; held-cards : list?
; Returns a new list of held-cards after the action discard is taken.
(define (fdiscard list-of-cards list-of-moves goal held-cards)
   (cdr held-cards))
; (fdis2 list-of-cards list-of-moves goal held-cards) -> pair?
; list-of-cards : list?
; list-of-moves : list?
; goal : number?
; held-cards : list?
; Returns the card that discarded.
 (define (fdis2 list-of-cards list-of-moves goal held-cards)
   (car held-cards))

 ;4.7--------------------------------------------------------------------
; (dostep alist2 alist1) -> list?
; alist2 : list?
; alist1 : list?
; Iterates one step
; alist1 represents held-cards and alist2 one step
(define (dostep alist2 alist1)
  (cond ((equal? (car alist2) 'draw) (append alist1 (cdr alist2)))
        ((equal? (car alist2) 'discard) (remove (cadr alist2) alist1))))
; (find-held-cards list-of-steps) -> list?
; list-of-steps : list?
; Returns the list of held-cards after the list-of-steps is applied
; Empty list is an accumulator for held-cards
(define (find-held-cards list-of-steps)
  (foldl dostep '() list-of-steps))

;4.8-----------------------------------------------------------------------------------
; (calc-playerpoint list-of-cards) -> number?
; list-of-cards : list?
; Calculates and returns the corresponding playerpoint for list-of-cards
(define (calc-playerpoint list-of-cards)
  (foldl + 0 (map card-rank list-of-cards)))

;4.6-------------------------------------------------------------------
; (pre-find-steps list-of-cards list-of-moves goal) -> list?
; list-of-cards : list?
; list-of-moves : list?
; goal : number?
; Returns a list of steps that is a list of pairs of moves and corresponding cards along the game and there could be "gameover" at the end of the list if hits a game-over statement
; Recursivly call itself starting from end of the list-of-moves. Base step is empty move list. It checks Game-over conditions after every draw step, before every discard step
(define (pre-find-steps list-of-cards list-of-moves goal) 
	(if (null? list-of-moves)
    	'()
    	;else part of first if
		(let ( [ pre-steps (pre-find-steps list-of-cards ( reverse ( cdr (reverse list-of-moves))) goal) ] ) 

			(cond 
				; If presteps is empty then we are in first move that must be 'draw' . Check Goal for Gameover
				((null? pre-steps) ( if (> (card-rank (car list-of-cards)) goal) (append pre-steps (list (list 'draw (car list-of-cards))) '(gameover) ) (append pre-steps (list (list 'draw (car list-of-cards )))) ))
				; If last element is gameover than does nothing
				( (equal? 'gameover (last pre-steps))  pre-steps)

				( else 
					(if (equal? (last list-of-moves) 'discard)
						; Checks if discard move can be done
		    			(if (null? (find-held-cards pre-steps))
		       	 			( append pre-steps '(gameover) )
 		        			(append pre-steps (list (list 'discard (fdis2 '() '() goal (find-held-cards pre-steps)))))
 		        		)
		    			;else part of second if
		    			(cond 
		    				;checks draw move can be done
		    				( (> (length (filter (lambda (x) (equal? x 'draw)) list-of-moves)) (length list-of-cards) ) (append pre-steps '(gameover) ) )
		    				;check for gameover 
		    				(else 
		    					(if (> (+ (card-rank (list-ref list-of-cards (- (length (filter (lambda (x) (equal? x 'draw)) list-of-moves)) 1)) ) (calc-playerpoint (find-held-cards pre-steps)) ) goal) 
		    						(append pre-steps (list (list 'draw (list-ref list-of-cards (- (length (filter (lambda (x) (equal? x 'draw)) list-of-moves)) 1)))) '(gameover))
		    						(append pre-steps (list (list 'draw (list-ref list-of-cards (- (length (filter (lambda (x) (equal? x 'draw)) list-of-moves)) 1)))))
		    			 		)
		    				)
		    			)
		    		)
				)
			)
		)
	)
)

 ; (find-steps list-of-cards list-of-moves goal)
 ; list-of-cards : list?
 ; list-of-moves : list?
 ; goal : number?
 ; Returns a list of steps that is a list of pairs of moves and corresponding cards along the game
 ; It removes "gameover" statement on pre-find-step
(define (find-steps list-of-cards list-of-moves goal)
  (remove 'gameover (pre-find-steps list-of-cards list-of-moves goal)))

;4.9---------------------------------------------------------------------------------------------
; (calc-prescore list-of-cards goal) -> number?
;  list-of-cards : list?
;  goal : number?
; Calculates and returns prescore
(define (calc-prescore list-of-cards goal)
  (let ([pscore (calc-playerpoint list-of-cards)])
  	(if (> pscore goal)
  	    (* 5 (- pscore goal))
  	    (- goal pscore))))
; (calc-score list-of-cards goal) -> number?
;  list-of-cards : list?
;  goal : number?
; Calculates and returns finalscore
(define (calc-score list-of-cards goal)
  (let ([prescore (calc-prescore list-of-cards goal)])
  	(if (all-same-color list-of-cards)
  	    (quotient prescore 2)
  	    prescore)))

;4.10---------------------------------------------------------------------------------------
; (play list-of-cards list-of-moves goal) -> number?
; list-of-cards : list?
; list-of-moves : list?
; goal : number?
; Returns finalscore at the end of the game after processing (some or all of) the moves in the move list in order
(define (play list-of-cards list-of-moves goal)
  (calc-score (find-held-cards (find-steps list-of-cards list-of-moves goal)) goal))
