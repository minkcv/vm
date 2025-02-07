"Vim syntax file
"Language:          Vaporlang
"Maintainer:        Will Smith <minkcv@gmail.com>
"Latest Revision:   2025-2-6
"How to use:
"create ~/.vim/ftdetect/vapor.vim
"autocmd BufRead,BufNewFile *.vapor set filetype=vapor
"
"add to ~/.vimrc
"syntax on
"
"exec
"mkdir ~/.vim/syntax
"cd .vim/syntax
"ln -s ~/vm/etc/vapor.vim vapor.vim

if exists("b:current_syntax")
    finish
endif

syntax clear
syntax case match

syntax keyword vaporStatement call return halt func
syntax keyword vaporDebug break
syntax keyword vaporConditional if while
syntax keyword vaporType const var
syntax keyword vaporTodo TODO contained

syntax match vaporIdentifier "\<[A-Za-z]+\>"
syntax match vaporNumber "\<0[xX]\x\+\>"
syntax match vaporNumber "\<\d\+\>"
syntax match vaporComment "//.*$" contains=vaporTodo

hi def link vaporStatement Statement
hi def link vaporIdentifier Identifier
hi def link vaporConditional Conditional
hi def link vaporTodo Todo
hi def link vaporDebug Debug
hi def link vaporType Type
hi def link vaporNumber Number
hi def link vaporComment Comment

let b:current_syntax = "vapor"

