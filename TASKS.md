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

- [x] **Implement parser for `||` and `&&` (bonus)**  
  **Owner:** `@gm-infinite`  
  **Progress:** [▮▮▮▮▮▮▮▮▮▮]  
  **Difficulty:** Medium-Hard kolay gelsin.  
                  Mart 11: adam yapmış abi... -Emir

- [ ] **Implement parser for single pipe (`|`)**  
  **Owner:** `@thecloudrazor`  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** Medium  
  **Additional Notes:** Parsing and command handling will be re-implemented in accordance with the logic of Pipex project. But, there's going to be a global struct which holds both the total composite command, as splitted char* values, inside a char**, and a size variable indicating how many we are dealing with. Parser will take a `t_split *` as an argument and access the char**. In each child, necessary parts will be passed to the execve command, via the help of some additional helper functions (which I will create when needed).

- [ ] **Implement parser for redirection operators (`<`, `<<`, `>`, `>>`)**  
  **Owner:** `@thecloudrazor`  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** Easy-Medium  
  **Additional Notes:** Input and output redirections, as well as Here Document functionality will be re-implemented in accordance with the logic of Pipex project. 

- [ ] **Implement parser for quotes and environment variable syntax (`''`, `""`, `$environment_variables`)**  
  **Owner:** `@thecloudrazor`
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** Medium-Hard  
  **Additional Notes:** Quotes are already implemented in the current version of Pipex. Only some minor adjustments will be needed. However, they are not implemented in the parser. This task will be about implementing the parser for environment variables. It will need to be able to handle environment variables.
  
  Kuzey: It seems like we misunderstood the evaluation page. We were about to try and implement **`$""`**, which isn't needed. What was meant in the page is to parse environment variables within the "...". (ex/ '$HOME' = $HOME, "$HOME" = /home/42login)  
    - [ ] Environment variables:                  `$environment_variable`  
  CAREFUL: We will need to consider the '$' scenario!  

- [ ] **Implement wildcard (`*`) parser (bonus)**  
  **Owner:** `@gm-infinite`  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

---

## Execve & Built-in Commands

- [ ] **Implement command execution (including built-in commands)**  
  **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
  **Difficulty:** —

  **Built-in Commands:**
  - [ ] **Implement `echo` with `-n` support**  
    **Owner:** `@gm-infinite`  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —  

  - [ ] **Implement `exit`**  
    **Owner:** `@gm-infinite`  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —  

  - [ ] **Implement `cd`**  
    **Owner:** `@thecloudrazor`  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —  
    **Additional Notes:**  
    - **`opendir(".")`** opens the current directory.  
    - **`readdir(dir)`** reads each entry (file or directory) inside it.  
    - **`closedir(dir)`** closes the directory stream. 

  - [ ] **Implement `pwd`**  
    **Owner:** `@gm-infinite`  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —  
    **Additional Notes:** USE getcwd()! It IS allowed.  

  - [ ] **Implement `export`**  
    **Owner:** `@thecloudrazor`  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —  

  - [ ] **Implement `unset`**  
    **Owner:** `@thecloudrazor`  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —  

  - [ ] **Implement `env`**  
    **Owner:** `@thecloudrazor`  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —  

  - [ ] **Handle `$?` (exit status)**  
    **Owner:** `@thecloudrazor`  
    **Progress:** [▮▯▯▯▯▯▯▯▯▯]  
    **Difficulty:** —

- ## EXTRA NOTES
  - april 8th, 2025
    - sep_opt_arg.c must be revised to comply with non-bonus requirements of the project. (|| and && won't be checked).

  - march 22:
    - cat a.txt > out.txt | cat << eof            --> it doesnt created or modifty out.txt until here_doc is satisfied.
    - cat << 'eof' --> inputs given: hello, $HOME --> output:hello $HOME (not HOME equivelent in env just "$HOME")
    - when $HOME or similar enviroment variables are given to here_doc, if the deliminer is in quotes it takes the as literal.
    - cat << eof --> inputs given: inputs given: hello, $HOME --> output:hello /home/username
    
  - March 16:
    - echo kuzey > output.txt berk                --> creates file output.txt and puts kuzey berk in to it.
    - echo kuzey > output.txt berk > outfile2.txt --> creates both output.txt and outfile2.txt but only puts kuzey berk in outfile2.txt.

  - March 11: these additional conditions seem to be working and they need to be considered carefully.
    - (echo Kuzey | grep K) && echo Emir    -->   pipe inside paranthesis AND also before another execution with &&
    - (echo Kuzey || echo emir) | grep e    -->   pipe after paranthesis, taking UNIX SHELL OR's output
    - Emir: regex ve math.exp.lere bakacağım. (tamamen bireysel araştırma konusu)

  - commands work like this too:
    - "e""c""h""o" hello -> prints hello with newline
    - "e"c"ho" hello     -> prints hello with newline
    - "e"c "ho" hello    -> zsh: command not found: ec
    - "l""s"             -> same output as ls command
    - as long as theres no seperating character for " or ' they count as one word

  - echo with -n has weird interactions:
    - echo -n hello      ->prints hello with NO newline
    - echo -nnnnnn hello ->prints hello with NO newline
    - echo hello -n      ->prints "hello -n" with newline
    - echo -nk hello     ->prints "-nk hello" with newline
    - echo -n -nn -nnn a ->prints "a" without newline
    - echo -n -nk -nnn a ->prints "-nk -nnn a" without newline
    - echo -n -nn -nkn a ->prints "-nkn a" without newline
---
