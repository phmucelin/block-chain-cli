# Block Chain em C

O que é, funções necessárias, estruturas.

## Uma breve explicação sobre o que é BlockChain

* A blockchain registra informações como: a quantia de bitcoins (ou outras moedas) transacionadas,
quem enviou, quem recebeu, quando essa transação foi feita e em qual lugar do livro ela está registrada. 
Isso mostra que a transparência é um dos principais atributos da blockchain.
* Ela armazena as informações de um grupo de transações em blocos,
marcando cada bloco com um registro de tempo e data. A cada período de tempo (10 minutos no blockchain), 
é formado um novo bloco de transações, que se liga ao bloco anterior.

## Structs necessárias
 - X User -> Nome, UUID, coins, int qtdCoints, 
 - Banco -> Nome, Registro, Number_Clients. 
 - X Transação ->
 - Block ->
 - Exchange ->
 - X ENUM | Coins ->
 - 

## Funções que serão necessárias inicialmente

- X generate_UUID - Irá gerar o ID de cada transação de forma única, facilitando cruzar transacoes.
- X new_user - Cada usuário terá seu ID, carteira de moedas (enum), senha com Hash (Segurança), Nome, Data de Nascimento, qtd de cada moeda.
- X new_bank - Teremos bancos cadastrados para que seja possível realizar o withdraw.
- X delete_user - Essa funcao ira deletar totalmente o usuario de nossa base, no entanto irá chamar a transfer ou withdraw, caso ele tenha moedas, para verificar a existencia de moedas, será chamado Check_Amount. O UUID será liberado para um novo usuário.
- X Check_Amount - Ira verificar se o usuário realmente pode realizar a transacao, check direto em seu saldo, validando moedas.
- X merge_Coins - Caso o usuario queira fazer uma transacao, no entanto nao tenha o valor cheio em apenas uma moeda, ele ira poder juntar todas as suas moedas em apenas uma de sua preferencia (Lembrando que cada moeda tem seu valor, ex: C Coin pode valer 1/2 da Pedro Coin, entao terá o ajuste).
- X Check_Transaction - Para evitar a chamada de muitas funções separadamente dentro da funcao Transaction, teremos a Check_Transaction, que irá cuidar de validar totalmente o envio da moeda para o outro usuário. Obs: Valores enviados incorretamente não poderão receber reembolso, apenas caso o usuário que recebeu queira devolver.
- X Transaction - Realiza a transacao de moedas entre dois usuarios, chamando a Check_Transaction, anteriormente de confirmar. Sera uma funcao bool.
- X List_Users - Funcao que ira permitir listar todos os usuarios cadastrados dentro do sistema, porém sem consultar o saldo prévio de cada um, pois isso será particular, apenas recebendo, nome, data de nasc e uuid.
- X save_user_infos_db - Salvar todas as infos dos usuarios dentro de um Banco de Dados.
- X save_transactions_info_db - Salvar todas transacoes, amount, id do user que envia e id de user que recebe. 
- X register_exchanges - Funcao acessada apenas pelo administrador do servidor, para fornecer opcoes ao user que deseja fazer withdraw, cada exchange terá seu padrão, algumas irão pedir amount, user info, motivo e etc. 
- X fill_block - Funcao chamada a cada 10 minutos de execução, dentro de cada 10 minutos irá entrar todas as transações feitas dentro deste periodo, entao caso ocorram 20 transações 08:00 e 08:10, teremos 20 transações dentro desse block.
- X create_block - Criar block recebendo transacoes, tempo de inicio e final, respeitando o time de 10 minutos.
- X buy_coins_with_money - Funcao para comprar qualquer coin, chamando check_amount com pagamento via Money.
- X recharge_money - Funcao para dar recharge com dinheiro real, banco, pix, transfer.
- X transfer_from_exchange - Funcao para receber MONEY atraves de outra exchange, nao RECEBE CRYPTO.
- X withdraw_money - Saque direto para banco pessoal.
- X getCoin_price - Receber o valor da coin em tempo real, retorna double.   
- X verify_admin - Verifica se usuario é um admin ou nao, com base no UUID, previamente cadastrado em um .bat.
- 
## Estrutura Adotadas

- Para os blocos de transações serão adotados listas encadeadas, lincando um ao outro sequencialmente.
- Para multiplas transações em curtos período de tempo, iremos realizar com Filas, não será possível disparar muitas transações ao mesmo tempo, por questões de segurança.
- Para os registros dos usuários iremos utilizar Hashmap, nas senhas teremos Hash padrão, 256.

