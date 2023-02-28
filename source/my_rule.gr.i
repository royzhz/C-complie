================================= Productions =================================
AAA                         ->          Program 0
Program                     ->          Functions 0
Functions                   ->          Function Functions 0
Functions                   ->          Function 0
Paramete_list               ->          @ 0
Paramete_list               ->          Paramete_list , TYPE id 0
Paramete_list               ->          TYPE id 0
Function                    ->          TYPE id ( Paramete_list ) Create_variable Function_block 0
Create_variable             ->          @ 22
TYPE                        ->          int 0
TYPE                        ->          float 0
TYPE                        ->          void 0
Function_block              ->          { } 0
Function_block              ->          Statement_block 21
Statement_block             ->          { variable_new_layer Statements } 24
Statement_block             ->          { } 0
variable_new_layer          ->          @ 23
Statements                  ->          Statements M Statement 20
Statements                  ->          Statement 1
Statement                   ->          Declarative_sentence 1
Statement                   ->          Assignment_sentence 1
Statement                   ->          Call_Function_sentence 1
Statement                   ->          If_sentence 1
Statement                   ->          While_sentence 1
Statement                   ->          Return_sentence 1
Statement                   ->          Statement_block 1
Call_Function_sentence      ->          id ( Parameter_list ) ; 0
Declarative_sentence        ->          TYPE id ; 5
Declarative_sentence        ->          TYPE id = EQUATION ; 6
Assignment_sentence         ->          id = EQUATION ; 4
Return_sentence             ->          return EQUATION ; 0
Return_sentence             ->          return ; 0
While_sentence              ->          while M ( E ) M Statement_block 19
If_sentence                 ->          if ( E ) M Statement_block 17
If_sentence                 ->          if ( E ) M Statement_block N else M Statement_block 18
E                           ->          C_and or M C_and 9
E                           ->          C_and 1
C_and                       ->          C_not and M C_not 10
C_and                       ->          C_not 1
C_not                       ->          not C_end 11
C_not                       ->          C_end 1
C_end                       ->          C_ADD relop C_ADD 14
C_end                       ->          C_ADD 16
C_ADD                       ->          C_ADD + C_MUL 3
C_ADD                       ->          C_ADD - C_MUL 3
C_ADD                       ->          C_MUL 1
C_MUL                       ->          C_MUL * C_END 3
C_MUL                       ->          C_MUL / C_END 3
C_MUL                       ->          C_END 1
C_END                       ->          ( C_ADD ) 2
C_END                       ->          id 7
C_END                       ->          const_int 15
M                           ->          @ 13
N                           ->          @ 12
EQUATION                    ->          EQUATION or EQUATION 3
EQUATION                    ->          EQUATION and EQUATION 3
EQUATION                    ->          not EQUATION 3
EQUATION                    ->          E_Expression 1
E_Expression                ->          @ 0
E_Expression                ->          E_Add_Expression 1
E_Expression                ->          E_Add_Expression relop E_Add_Expression 3
E_Add_Expression            ->          E_Item 1
E_Add_Expression            ->          E_Item + E_Add_Expression 3
E_Add_Expression            ->          E_Item - E_Add_Expression 3
E_Item                      ->          E_divisor 1
E_Item                      ->          E_divisor * E_divisor 3
E_Item                      ->          E_divisor / E_divisor 3
E_divisor                   ->          ( EQUATION ) 2
E_divisor                   ->          id 7
E_divisor                   ->          const_int 15
relop                       ->          > 8
relop                       ->          >= 8
relop                       ->          < 8
relop                       ->          <= 8
relop                       ->          == 8
relop                       ->          != 8
pdc num: 76
================================= tokens =================================
!=
(
)
*
+
,
-
/
;
<
<=
=
==
>
>=
@
AAA
Assignment_sentence
C_ADD
C_END
C_MUL
C_and
C_end
C_not
Call_Function_sentence
Create_variable
Declarative_sentence
E
EQUATION
E_Add_Expression
E_Expression
E_Item
E_divisor
Function
Function_block
Functions
If_sentence
M
N
Paramete_list
Parameter_list
Program
Return_sentence
Statement
Statement_block
Statements
TYPE
While_sentence
and
const_int
else
float
id
if
int
not
or
relop
return
variable_new_layer
void
while
{
}
token num: 64
================================= NonTerminals No =================================
AAA                 :           0 
Assignment_sentence :           1 
C_ADD               :           2 
C_END               :           3 
C_MUL               :           4 
C_and               :           5 
C_end               :           6 
C_not               :           7 
Call_Function_sentence:           8 
Create_variable     :           9 
Declarative_sentence:           10 
E                   :           11 
EQUATION            :           12 
E_Add_Expression    :           13 
E_Expression        :           14 
E_Item              :           15 
E_divisor           :           16 
Function            :           17 
Function_block      :           18 
Functions           :           19 
If_sentence         :           20 
M                   :           21 
N                   :           22 
Paramete_list       :           23 
Program             :           24 
Return_sentence     :           25 
Statement           :           26 
Statement_block     :           27 
Statements          :           28 
TYPE                :           29 
While_sentence      :           30 
relop               :           31 
variable_new_layer  :           32 
nonterm num: 33
================================= Terminals No =================================
!=                  :           0 
(                   :           1 
)                   :           2 
*                   :           3 
+                   :           4 
,                   :           5 
-                   :           6 
/                   :           7 
;                   :           8 
<                   :           9 
<=                  :           10 
=                   :           11 
==                  :           12 
>                   :           13 
>=                  :           14 
@                   :           15 
Parameter_list      :           16 
and                 :           17 
const_int           :           18 
else                :           19 
float               :           20 
id                  :           21 
if                  :           22 
int                 :           23 
not                 :           24 
or                  :           25 
return              :           26 
void                :           27 
while               :           28 
{                   :           29 
}                   :           30 
term num: 31
================================= First Sets =================================
!=                  :           != 
(                   :           ( 
)                   :           ) 
*                   :           * 
+                   :           + 
,                   :           , 
-                   :           - 
/                   :           / 
;                   :           ; 
<                   :           < 
<=                  :           <= 
=                   :           = 
==                  :           == 
>                   :           > 
>=                  :           >= 
@                   :           @ 
AAA                 :           float int void 
Assignment_sentence :           id 
C_ADD               :           ( const_int id 
C_END               :           ( const_int id 
C_MUL               :           ( const_int id 
C_and               :           ( const_int id not 
C_end               :           ( const_int id 
C_not               :           ( const_int id not 
Call_Function_sentence:           id 
Create_variable     :           @ 
Declarative_sentence:           float int void 
E                   :           ( const_int id not 
EQUATION            :           ( @ const_int id not 
E_Add_Expression    :           ( const_int id 
E_Expression        :           ( @ const_int id 
E_Item              :           ( const_int id 
E_divisor           :           ( const_int id 
Function            :           float int void 
Function_block      :           { 
Functions           :           float int void 
If_sentence         :           if 
M                   :           @ 
N                   :           @ 
Paramete_list       :           @ float int void 
Parameter_list      :           Parameter_list 
Program             :           float int void 
Return_sentence     :           return 
Statement           :           float id if int return void while { 
Statement_block     :           { 
Statements          :           float id if int return void while { 
TYPE                :           float int void 
While_sentence      :           while 
and                 :           and 
const_int           :           const_int 
else                :           else 
float               :           float 
id                  :           id 
if                  :           if 
int                 :           int 
not                 :           not 
or                  :           or 
relop               :           != < <= == > >= 
return              :           return 
variable_new_layer  :           @ 
void                :           void 
while               :           while 
{                   :           { 
}                   :           } 
================================= Follow Sets =================================
AAA                 :           $ 
Assignment_sentence :           float id if int return void while { } 
C_ADD               :           != ) + - < <= == > >= and or 
C_END               :           != ) * + - / < <= == > >= and or 
C_MUL               :           != ) * + - / < <= == > >= and or 
C_and               :           ) or 
C_end               :           ) and or 
C_not               :           ) and or 
Call_Function_sentence:           float id if int return void while { } 
Create_variable     :           { 
Declarative_sentence:           float id if int return void while { } 
E                   :           ) 
EQUATION            :           ) ; and or 
E_Add_Expression    :           != ) ; < <= == > >= and or 
E_Expression        :           ) ; and or 
E_Item              :           != ) + - ; < <= == > >= and or 
E_divisor           :           != ) * + - / ; < <= == > >= and or 
Function            :           $ float int void 
Function_block      :           $ float int void 
Functions           :           $ 
If_sentence         :           float id if int return void while { } 
M                   :           ( const_int float id if int not return void while { 
N                   :           else 
Paramete_list       :           ) , 
Program             :           $ 
Return_sentence     :           float id if int return void while { } 
Statement           :           float id if int return void while { } 
Statement_block     :           $ else float id if int return void while { } 
Statements          :           float id if int return void while { } 
TYPE                :           id 
While_sentence      :           float id if int return void while { } 
relop               :           ( const_int id 
variable_new_layer  :           float id if int return void while { 
