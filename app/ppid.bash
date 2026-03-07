# bash completion for ppid (bas-c)
_ppid() {
  local cur prev opts
  _get_comp_words_by_ref -n : cur prev
  opts='--self --all --quiet --verbose --version -s -a -q -v'
  if [[ $cur == -* ]]; then
    COMPREPLY=($(compgen -W "$opts" -- "$cur"))
  else
    # complete PIDs from /proc
    COMPREPLY=($(compgen -W '$(ls -1 /proc 2>/dev/null | grep -E "^[0-9]+$")' -- "$cur"))
  fi
}
complete -F _ppid ppid
