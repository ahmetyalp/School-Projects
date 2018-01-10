% Class scheduler prolog program 
:-dynamic student/2.
:-dynamic available_slots/1.
:-dynamic room_capacity/2.

% keep all courses dynamic to not calculate it again.
:-dynamic allCourses/1.
% keep all produce(X,Y) dynamic in produced.
:-dynamic produced/2.

% returns set of all students.
all_students(X):-setof(C,B^student(C,B),X).
% checks wheter C is course that taken by any student or not.
isCourse(C):-student(_,X),member(C,X).
% returns set of all courses.
all_courses(X):-allCourses(X),!;setof(C,isCourse(C),X),asserta(allCourses(X)).
% checks student S takes course C.
isStudentTakeCourse(S,C):-student(S,B),member(C,B).
% for given C, L is the list of students that take course C.
student_list(C,L):-setof(S,isStudentTakeCourse(S,C),L).
% for given C, calculates number of students who take that course.
student_count(C,S):-student_list(C,L),length(L,S).
% checks student S takes both courses C1 and C2.
isStudentTakeCourses(S,C1,C2):-student(S,B),mmember(C1,B),mmember(C2,B).
% for given C1 and C2 , L is the list of students that take courses C1 and C2.
common_student_list(C1,C2,L):-setof(S,C1^C2^isStudentTakeCourses(S,C1,C2),L).
% for given C1,C2, S is the number of students who tahe both courses. If courses are not valid returns fail.
common_students(C1,C2,S):-(common_student_list(C1,C2,L),length(L,S)-> true ;all_courses(X),mmember(C1,X),mmember(C2,X),S=0).
% checks that X is member of Y, stops after finding X in Y 
mmember(X,Y):-member(X,Y),!.
% checks and produces one slot that on the available_slots.
check_slot(Z):-available_slots(W),member(Z,W).
% produces valid tuple that means that Course can fit in Room.
valid_tuple([X,Y,Z]):-check_slot(Z),room_capacity(Y,C),student_count(X,S),S=<C.
% base for produce
produce([],Y):-produced([],Y),!;bagof(Z,valid_tuple(Z),Y),assert(produced([],Y)).
% produces valid [Course,Place,Time] that can be added to [H|T] without any error.
produce([H|T],Y):-produced([H|T],Y),!;bagof(Z,W^(produce(T,W),member(Z,W),check_tuple(Z,H)),Y),assert(produced([H|T],Y)).%,compare(<,Y,H).
% for given X, Y is the list that includes X and one more tuple tuple that can be added X without an error.
f(X,Y):-produce(X,Q),member(H,Q),append([H],X,Y).
% for given N,X, Y is the list that includes X and N more tuple tuple that can be added X without an error.
g(N,X,Y):-N>1,f(X,Z),M is N-1,g(M,Z,Y).
% base for g(X,Y,Z).
g(1,X,Y):-f(X,Y).
% produces Final Plan. S is length of a final plan because every course should be placed one place and predicate g adds S [Course,Room,Time] tuple to F means F is final_plan.
final_plan(F):-all_courses(X),length(X,S),g(S,[],F).
% checks two tuple wheter they crate an error or not.
check_tuple([X,_,_],[X,_,_]):-true,!,false.
check_tuple([_,_,Z1],[_,_,Z2]):- \+compare(=,Z1,Z2),true,!.
check_tuple([_,Y,Z],[_,Y,Z]):-true,!,false.
check_tuple([X1,_,Z],[X2,_,Z]):-common_students(X1,X2,S),S=:=0.
% base case for errors_for_plan.
errors_for_plan([],0).
% First calculate tail's errors, then head's error due to room capacity finally checks head and tail's each element for determine there is a student who have more than one final in one slot. 
errors_for_plan([[X,Y,Z]|T],EC):-errors_for_plan(T,A),room_capacity(Y,Q),student_count(X,W),( (W-Q)>0 -> B is W-Q ; B=0),errors_for_tuple([X,Y,Z],T,C),EC is A+B+C.
% base case.
errors_for_tuple(X,[],0):-true,!.
% checks first parameter and second parameter's each element for determine there is a student who have more than one final in one slot. 
errors_for_tuple([X,Y,Z],[[XX,YY,ZZ]|T],E):-errors_for_tuple([X,Y,Z],T,A),( Z=ZZ -> common_students(X,XX,B) ; B=0 ),E is A+B.
%clears consulted knowledge base.
clear_knowledge_base:-retractall(produced(X,Y)),retractall(allCourses(Z)),all_students(S),length(S,L),write('student/2:'),retractall(student(A,B)),writeln(L),writeln('available_slots/1:1'),retractall(available_slots(C)),setof(R,H^room_capacity(R,H),RR),length(RR,LL),write('room_capacity/2:'),writeln(LL),retractall(room_capacity(O,P)).


