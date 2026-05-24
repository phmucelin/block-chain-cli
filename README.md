# Blockchain CLI em C

> Blockchain de criptoativos implementada do zero em C puro, com persistência em
> PostgreSQL e cotações em tempo real via CoinGecko. Projeto de estudo focado em
> gerenciamento manual de memória, estruturas de dados encadeadas e integração com
> bibliotecas nativas.

[![Linguagem](https://img.shields.io/badge/C-C11-blue.svg)](#)
[![Banco](https://img.shields.io/badge/PostgreSQL-16-336791.svg)](#)
[![Container](https://img.shields.io/badge/Docker-Compose-2496ED.svg)](#)
[![Build](https://img.shields.io/badge/build-Makefile-orange.svg)](#)

---

## Visão geral

A blockchain registra transações (remetente, destinatário, ativo, quantidade e
timestamp) em **blocos imutáveis encadeados por hash** — cada bloco carrega o hash
do anterior, formando uma cadeia onde qualquer alteração passada invalida todos
os blocos seguintes. Nesta implementação os blocos são fechados por **janela
temporal** (`expire_at`): as transações enfileiradas durante o período viram um
único bloco encadeado ao anterior.

---

## Decisões técnicas

### Estruturas de dados
- **Lista encadeada** para a cadeia de blocos (`Block* prox`) e para a carteira
  multi-moeda de cada usuário (`UserCoin* prox`) — escolhida pela natureza
  append-only de ambos e por permitir crescimento sem realocação.
- **Fila FIFO** (`Queue` com `first` / `last`) para o mempool de transações
  pendentes, garantindo ordem de processamento e evitando disparos simultâneos.
- **Tabela de relação N:N** (`relation_transaction_users`) entre usuários e
  moedas no banco, separando saldo de cada cripto da entidade principal `users`.
- **Encadeamento por hash** com campos `previous_hash[65]` e `hash[65]` em cada
  bloco — espaço para SHA-256 hex + terminador.

### Memória e ponteiros
- Alocação manual de todas as structs (`malloc` / `free`), com `free` em cascata
  ao remover usuários, transações e blocos.
- Forward declarations e separação de headers para quebrar dependências
  circulares entre `user_model`, `transaction_model` e `block_model`.
- Buffers fixos para hashes e UUIDs evitando alocação dinâmica desnecessária.

### Segurança
- Senhas armazenadas como **hash** (nunca em texto plano) e leitura sem eco no
  terminal via `termios` (`c_lflag &= ~ECHO`).
- Função de hash interna baseada em **FNV-1a** para strings genéricas.
- Verificação de admin por UUID antes de qualquer operação sensível
  (cadastro de banco, exchange, etc.).
- Validação de remetente, destinatário e saldo antes de enfileirar transação.

### Integrações externas
- **`libpq`** para comunicação direta com PostgreSQL (sem ORM), incluindo
  criação de tabelas, `INSERT` parametrizado e `SELECT` com carregamento das
  structs em memória no boot.
- **`libcurl`** para consumir a API pública da CoinGecko e obter preços de
  BTC/ETH/USDT em tempo real, com inicialização única via `atexit`.
- **Docker Compose** para subir o Postgres local de forma reproduzível.

### Organização do código
Separação em camadas com headers dedicados em `models_functions/` para isolar
contratos das implementações em `services/`:

```
cli/        -> menus, autenticação, fluxo de telas
models/     -> structs do domínio (sem lógica)
services/   -> regras de negócio puras
db/         -> persistência (libpq + DDL)
```

---

## Funcionalidades

| Módulo            | O que faz                                                                |
| ----------------- | ------------------------------------------------------------------------ |
| **Usuários**      | Cadastro com CPF, UUID, senha hasheada, banco e idade. Login autenticado. |
| **Carteira**      | Saldo em dinheiro + lista encadeada de moedas (BTC, ETH, USDT).          |
| **Transações**    | Criação, validação, enfileiramento e consolidação em bloco.              |
| **Blocos**        | Encadeamento por hash, controle por janela temporal, contagem de transações. |
| **Cotação**       | Preço em tempo real via CoinGecko (`libcurl` + parsing de JSON).         |
| **Conversão**     | `merge_coins` converte entre moedas usando cotação de mercado.           |
| **Compra**        | `buy_coins` debita saldo em dinheiro e credita cripto.                   |
| **Exchange/Bank** | Cadastro de bancos e exchanges (restrito ao admin).                      |
| **Admin**         | Identificação por UUID + `verify_admin` em operações sensíveis.          |
| **Persistência**  | Tabelas `users`, `transactions`, `blocks` e relação N:N usuário ↔ moeda. |

---

## Stack

| Camada          | Tecnologia                            |
| --------------- | ------------------------------------- |
| Linguagem       | C (C11)                               |
| Banco           | PostgreSQL 16 (Alpine, via Docker)    |
| Driver SQL      | `libpq`                               |
| HTTP            | `libcurl`                             |
| Testes          | [Unity](http://www.throwtheswitch.org/unity) |
| Build           | GNU Make                              |
| API de cotação  | CoinGecko (pública)                   |

---

## Modelo de dados

```sql
users (
  uuid       VARCHAR(37) PRIMARY KEY,
  cpf        VARCHAR(14) NOT NULL UNIQUE,
  name       VARCHAR(64) NOT NULL,
  hashpass   VARCHAR(64) NOT NULL,
  age        INTEGER     NOT NULL,
  balance    DOUBLE PRECISION NOT NULL DEFAULT 0,
  bank_name  VARCHAR(64) NOT NULL
)

transactions (
  id        SERIAL PRIMARY KEY,
  sender    VARCHAR(64) NOT NULL,
  receiver  VARCHAR(64) NOT NULL,
  qtdCoins  DOUBLE PRECISION NOT NULL,
  coinType  VARCHAR(8)  NOT NULL,
  timestamp TIMESTAMP   NOT NULL,
  receipt   INTEGER     NOT NULL UNIQUE
)

blocks (
  id                   SERIAL PRIMARY KEY,
  blockHash            VARCHAR(64) NOT NULL UNIQUE,
  previousHash         VARCHAR(64),
  timestamp            TIMESTAMP   NOT NULL,
  transactionsID       VARCHAR(64),
  numberOfTransactions INTEGER,
  createdAt            TIMESTAMP,
  updatedAt            TIMESTAMP,
  expireAt             TIMESTAMP
)

relation_transaction_users (
  userId    SERIAL PRIMARY KEY,
  coin_name VARCHAR(20) NOT NULL,
  qtd_coin  DOUBLE PRECISION NOT NULL
)
```

---

## Como executar

```bash
# 1. Subir o Postgres
docker compose up -d

# 2. Compilar
make blockchain

# 3. Rodar
./blockchain
```

**Pré-requisitos:** GCC, Make, Docker, `libpq` e `libcurl`.
No macOS: `brew install libpq curl`. O caminho do `libpq` está fixado em
`/opt/homebrew/opt/libpq` no Makefile — ajuste a variável `LIBPQ` se necessário.

O Postgres sobe em `localhost:5434` com usuário/senha `blockchain` / `blockchain123`
e database `blockchain_db`. As tabelas são criadas automaticamente no primeiro boot.
