# Project Task List

## Main

- [x] **Take input and display a prompt before parsing anything**  
  **Owner:** `@gm-infinite`  
  **Progress:** [▮▮▮▮▮▮▮▮▮▮]  
  **Difficulty:** easy

- [x] **Have a working history (skip adding empty strings)**  
  **Owner:** `@gm-infinite`  
  **Progress:** [▮▮▮▮▮▮▮▮▮▮]  
  **Difficulty:** easy

- [ ] **Implement a safe_exit function (for all exit scenarios, e.g., ctrl-D)**  
  **Owner:** —  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

- [ ] **Ensure proper memory free code at program termination**  
  **Owner:** —  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

- [x] **Import libft**  
  **Owner:** `@thecloudrazor`  
  **Progress:** [▮▮▮▮▮▮▮▮▮▮]  
  **Difficulty:** easy

---

## Signals

- [ ] **Handle ctrl + C**  
  **Owner:** —  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

- [x] **Handle ctrl + D**  
  **Owner:** `@gm-infinite`  
  **Progress:** [▮▮▮▮▮▮▮▮▮]  
  **Difficulty:** easy

- [ ] **Handle ctrl + \\**  
  **Owner:** —  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

- [ ] **Ensure proper handling of child processes when dealing with signals**  
  **Owner:** —  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

---

## Parser

- [x] **Creation and implementation of the t_split struct**  
  **Owner:** `@gm-infinite`  
  **Progress:** [▮▮▮▮▮▮▮▮▮▮]  
  **Difficulty:** Easy-Medium

- [ ] **Implement parser for `||` and `&&` (bonus)**  
  **Owner:** `@gm-infinite`  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** Medium

- [ ] **Implement parser for single pipe (`|`)**  
  **Owner:** `@thecloudrazor`  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** Medium  
  **Additional Notes:** Parsing and command handling will be re-implemented in accordance with the logic of Pipex project. But, there's going to be a global struct which holds both the total composite command, as splitted char* values, inside a char**, and a size variable indicating how many we are dealing with. Parser will take a `t_split\*` as an argument and access the char**. In each child, necessary parts will be passed to the execve command, via the help of some additional helper functions (which I will create when needed).

- [ ] **Implement parser for redirection operators (`<`, `<<`, `>`, `>>`)**  
  **Owner:** `@thecloudrazor`  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** Easy-Medium
  **Additional Notes:** Input and output redirections, as well as Here Document functionality will be re-implemented in accordance with the logic of Pipex project. 

- [ ] **Implement parser for quotes and environment variable syntax (`''`, `""`, `$""`, `$environment_variables`)**  
  **Owner:** `@thecloudrazor`
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** Medium-Hard
  **Additional Notes:** Quotes are already implemented in the current version of Pipex. Only some minor adjustments will be needed. However, they are not implemented in the parser. This task will be about implementing the parser for environment variables. It will need to be able to handle environment variables.
    - [ ] Environment variables: `$environment_variable`
    - [ ] Environment variables in double quotes: `$"environment_variable"`

- [ ] **Implement wildcard (`*`) parser (bonus)**  
  **Owner:** —  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

---

## Execve & Built-in Commands

- [ ] **Implement command execution (including built-in commands)**  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

  **Built-in Commands:**
  - [ ] **Implement `echo` with `-n` support**  
    **Owner:** —  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —
  - [ ] **Implement `exit`**  
    **Owner:** —  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —
  - [ ] **Implement `cd`**  
    **Owner:** —  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —
  - [ ] **Implement `pwd`**  
    **Owner:** —  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —
  - [ ] **Implement `export`**  
    **Owner:** —  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —
  - [ ] **Implement `unset`**  
    **Owner:** —  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —
  - [ ] **Implement `env`**  
    **Owner:** —  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —

- [ ] **Handle `$?` (exit status)**  
  **Owner:** `@thecloudrazor`  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

---
