## Troubleshooting

### **Common Issues**

**Issue**: Command not found
```bash
minishell> mycommand
mycommand: command not found
```
**Solution**: Ensure the command is in your PATH or use absolute path

**Issue**: Permission denied
```bash
minishell> ./script.sh
./script.sh: Permission denied  
```
**Solution**: Check file permissions with `ls -l` and use `chmod +x` if needed

**Issue**: Pipe broken
```bash
minishell> cat largefile.txt | head
```
**Solution**: This is normal behavior when `head` closes early, terminating the pipeline

### **Memory Issues**
If you encounter memory-related problems:
1. Run with Valgrind: `valgrind --leak-check=full ./minishell`
2. Check for proper cleanup in signal handlers
3. Verify all malloc/free pairs are balanced
4. Review file descriptor closing in error paths

### **Signal Handling**
For signal-related issues:
- Ensure proper signal mask setup before fork()
- Verify signal handlers are async-signal-safe
- Check signal delivery to correct process groups
- Review signal blocking during critical sections

### **Debugging Tips**
1. **Enable Debug Mode**: Use debug builds for additional diagnostics
2. **Check Exit Codes**: Use `echo $?` to verify command exit status
3. **Trace Execution**: Add temporary printf statements for complex flows
4. **Memory Validation**: Regular Valgrind testing during development
5. **Signal Testing**: Test signal delivery at various execution points

---