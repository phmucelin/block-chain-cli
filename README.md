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
 - User -> Nome, UUID, coins, int qtdCoints, 
 - Banco -> Nome, Registro, Number_Clients. 
 - Transação ->
 - Block ->
 - Exchange ->
 - ENUM | Coins ->
 - 

## Funções que serão necessárias inicialmente

- generate_UUID - Irá gerar o ID de cada transação de forma única, facilitando cruzar transacoes.
- new_user - Cada usuário terá seu ID, carteira de moedas (enum), senha com Hash (Segurança), Nome, Data de Nascimento, qtd de cada moeda.
- new_bank - Teremos bancos cadastrados para que seja possível realizar o withdraw.
- delete_user - Essa funcao ira deletar totalmente o usuario de nossa base, no entanto irá chamar a transfer ou withdraw, caso ele tenha moedas, para verificar a existencia de moedas, será chamado Check_Amount. O UUID será liberado para um novo usuário.
- Check_Amount - Ira verificar se o usuário realmente pode realizar a transacao, check direto em seu saldo, validando moedas.
- merge_Coins - Caso o usuario queira fazer uma transacao, no entanto nao tenha o valor cheio em apenas uma moeda, ele ira poder juntar todas as suas moedas em apenas uma de sua preferencia (Lembrando que cada moeda tem seu valor, ex: C Coin pode valer 1/2 da Pedro Coin, entao terá o ajuste).
- Check_Transaction - Para evitar a chamada de muitas funções separadamente dentro da funcao Transaction, teremos a Check_Transaction, que irá cuidar de validar totalmente o envio da moeda para o outro usuário. Obs: Valores enviados incorretamente não poderão receber reembolso, apenas caso o usuário que recebeu queira devolver.
- Transaction - Realiza a transacao de moedas entre dois usuarios, chamando a Check_Transaction, anteriormente de confirmar. Sera uma funcao bool.
- List_Users - Funcao que ira permitir listar todos os usuarios cadastrados dentro do sistema, porém sem consultar o saldo prévio de cada um, pois isso será particular, apenas recebendo, nome, data de nasc e uuid.
- save_user_infos_db - Salvar todas as infos dos usuarios dentro de um Banco de Dados.
- save_transactions_info_db - Salvar todas transacoes, amount, id do user que envia e id de user que recebe. 
-  withdraw - User informa se quer enviar para alguma outra corretora ou sacar diretamente para o seu banco.
- register_exchanges - Funcao acessada apenas pelo administrador do servidor, para fornecer opcoes ao user que deseja fazer withdraw, cada exchange terá seu padrão, algumas irão pedir amount, user info, motivo e etc. 
- create_block - Funcao chamada a cada 10 minutos de execução, dentro de cada 10 minutos irá entrar todas as transações feitas dentro deste periodo, entao caso ocorram 20 transações 08:00 e 08:10, teremos 20 transações dentro desse block.
- buy_coins_with_money - Funcao para comprar qualquer coin, chamando check_amount com pagamento via Money.
- recharge_money - Funcao para dar recharge com dinheiro real, banco, pix, transfer.
- transfer_from_exchange - Funcao para receber MONEY atraves de outra exchange, nao RECEBE CRYPTO.
- withdraw_money - Saque direto para banco pessoal.
- getCoin_price - Receber o valor da coin em tempo real, retorna double. -> 
double getPreco(CoinType tipo) {
    switch(tipo) {
        case BTC: return 65000.0;
        case ETH: return 3000.0;
        case USDT: return 1.0;
        default: return 0.0;
    }
}
  
## Estrutura Adotadas

- Para os blocos de transações serão adotados listas encadeadas, lincando um ao outro sequencialmente.
- Para multiplas transações em curtos período de tempo, iremos realizar com Filas, não será possível disparar muitas transações ao mesmo tempo, por questões de segurança.
- Para os registros dos usuários iremos utilizar Hashmap, nas senhas teremos Hash padrão, 256.

