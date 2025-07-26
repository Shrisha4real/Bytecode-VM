#include "Scanner.h"

Scanner::Scanner(const std::string& src) : source(src), start(src.data()), current(src.data()), line(1) {}

Token Scanner::scan_token() {
    remove_white_spaces();
    this->start = this->current;
    if (is_at_end()) {
        return this->make_token(token_type::TOKEN_EOF);
    }
    char c = this->advance();
    if (std::isdigit(c)) return this->number();
    if (std::isalpha(c)) return this->identifier();


    switch (c) {
    case '(': return this->make_token(TOKEN_LEFT_PAREN);
    case ')': return this->make_token(TOKEN_RIGHT_PAREN);
    case '{': return this->make_token(TOKEN_LEFT_BRACE);
    case '}': return this->make_token(TOKEN_RIGHT_BRACE);
    case ';': return this->make_token(TOKEN_SEMICOLON);
    case ',': return this->make_token(TOKEN_COMMA);
    case '.': return this->make_token(TOKEN_DOT);
    case '-': {
        return this->make_token(TOKEN_MINUS);
        // check for another - in case of decrement operations

    }
    case '+': {
        return this->make_token(TOKEN_PLUS);
        //check for another + in case of increment operations
    }
    case '/': return this->make_token(TOKEN_SLASH);
    case '*': return this->make_token(TOKEN_STAR);
    case '!':
        return this->make_token(
            this->match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
        return this->make_token(
            this->match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
        return this->make_token(
            this->match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
        return this->make_token(
            this->match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"':
        return  this->string();
    }

    return this->error_token("Unexpected character\n");
};

bool Scanner::is_at_end() const {
    return *(this->current) == '\0';
}
char Scanner::advance() {
    this->current++;
    return *(this->current - 1);
}

bool Scanner::match(char expected) {
    if (this->is_at_end()) return false;
    if (*(this->current) != expected) return false;
    this->current++;
    return true;
}


void Scanner::remove_white_spaces() {
    bool val = true;
    while (val) {
        char c = this->peek();

        switch (c) {
        case ' ':
        case '\t':
        case '\r':
            //val = false;
            this->advance();
            break;
        case'\n':
            this->line++;
            this->advance();
            break;
        case '/':
            if (this->peek_next() == '/') {
                while (peek() != '\n' && !this->is_at_end()) {
                    this->advance();
                }
            }
            else return;
            break;
        default: return;

        }

    }
}
Token Scanner::error_token(const char* message) {
    Token token;
    token.type = token_type::TOKEN_ERROR;
    token.start = message;
    token.length = static_cast<int>(std::strlen(message));
    token.line = this->line;
    return token;

}Token Scanner::make_token(token_type type) {
    Token token;
    token.type = type;
    token.start = this->start;
    token.length = static_cast<int>(this->current - this->start);
    token.line = this->line;
    return token;

}


char Scanner::peek() {
    return *(this->current);
}

char Scanner::peek_next() {
    if (this->is_at_end())
        return '\0';
    return *(this->current + 1);
}

Token Scanner::string() {
    while (this->peek() != '"' && !this->is_at_end()) {
        this->advance();
    }
    if (this->is_at_end())
        return this->error_token("Unterminated string\t");
    this->advance();
    return this->make_token(token_type::TOKEN_STRING);

}

Token Scanner::number() {
    while (std::isdigit(peek())) this->advance();
    if (this->peek() == '.' && this->is_at_end()) {
        while (std::isdigit(peek())) this->advance();
    }
    return this->make_token(token_type::TOKEN_NUMBER);
}

Token Scanner::identifier() {
    while (std::isalpha(this->peek()) || std::isdigit(this->peek())) this->advance();
    return this->make_token(this->identifier_type());

}

token_type Scanner::identifier_type() {
    switch (this->start[0]) {
    case 'a': return this->check_keyword(1, 2, "nd", TOKEN_AND);
    case 'c': return this->check_keyword(1, 4, "lass", TOKEN_CLASS);
    case 'e': return this->check_keyword(1, 3, "lse", TOKEN_ELSE);
    case 'i': return this->check_keyword(1, 1, "f", TOKEN_IF);
    case 'n': return this->check_keyword(1, 2, "il", TOKEN_NIL);
    case 'o': return this->check_keyword(1, 1, "r", TOKEN_OR);
    case 'p': return this->check_keyword(1, 4, "rint", TOKEN_PRINT);
    case 'r': return this->check_keyword(1, 5, "eturn", TOKEN_RETURN);
    case 's': return this->check_keyword(1, 4, "uper", TOKEN_SUPER);
    case 'v': return this->check_keyword(1, 2, "ar", TOKEN_VAR);
    case 'w': return this->check_keyword(1, 4, "hile", TOKEN_WHILE);
    case 'f':
        if (this->current - this->start > 1) {
            switch (this->start[1]) {
            case 'a': return this->check_keyword(2, 3, "lse", TOKEN_FALSE);
            case 'o': return this->check_keyword(2, 1, "r", TOKEN_FOR);
            case 'u': return this->check_keyword(2, 1, "n", TOKEN_FUN);
            }
        }
        break;
    case 't':
        if (this->current - this->start > 1) {
            switch (this->start[1]) {
            case 'h': return this->check_keyword(2, 2, "is", TOKEN_THIS);
            case 'r': return this->check_keyword(2, 2, "ue", TOKEN_TRUE);
            }
        }
        break;
    }

    return TOKEN_IDENTIFIER;
}

token_type Scanner::check_keyword(int start, int length, const  char* rest, token_type type) {
    if (this->current - this->start == start + length &&
        memcmp(this->start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;

}
