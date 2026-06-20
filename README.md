# Trabalhos 1 e 2 de Linguagens de Programação e Compiladores

## Compilação

O projeto requer as seguintes ferramentas instaladas:

- flex >=2.5.0
- bison >=3.8.0
- g++ >=8
- make >=4

Para compilar, rode o comando a seguir

```sh
make
```

isso gerará um executável `lalg` na pasta do projeto.

## Uso

O executável oferece três comandos:
- `dump-tokens` lê o conteúdo de um arquivo ou da entrada padrão e imprime na tela todas as tokens encontradas
- `dump-ast` lê o conteúdo de um arquivo ou da entrada padrão e imprime na tela a árvore de sintaxe abstrata no formato de uma DSL.
- `run` lê o conteúdo de um arquivo ou da entrada padrão e o executa com um interpretador simples.


### Exemplos

Arquivo de entrada: input.txt
```
{programa simples que soma dois números}
program soma;

var a, b: real;

begin
  read(a, b);
  a := a + b;
  write(a);
end.
```
#### Imprimindo tokens
```sh
./lalg dump-tokens input.txt
```

```
program - program
soma - id
; - ;
var - var
a - id
, - ,
b - id
: - :
real - real
; - ;
begin - begin
read - read
( - (
a - id
, - ,
b - id
) - )
; - ;
a - id
:= - :=
a - id
+ - +
b - id
; - ;
write - write
( - (
a - id
) - )
; - ;
end - end
. - .
```

#### Imprimindo AST
```sh
./lalg dump-ast input.txt
```

```
Program(
  name: Identifier(soma)
  declarations: [
    VariableDeclaration(
      type: TypeAnnotation(real)
      identifiers: [
        Identifier(a)
        Identifier(b)
      ]
    )
  ]
  block: Block(
    ReadStatement(
      variables: [
        Identifier(a)
        Identifier(b)
      ]
    )
    VariableAssignment(
      variable: Identifier(a)
      value: BinaryExpression(
        operator: +
        left: Identifier(a)
        right: Identifier(b)
      )
    )
    WriteStatement(
      variables: [
        Identifier(a)
      ]
    )
  )
)
```


#### Interpretando código
```sh
echo "1 2" | ./lalg run input.txt
```

```
3
```

