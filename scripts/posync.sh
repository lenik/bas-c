#!/usr/bin/env bash
# Sync bas_c.pot / *.po from POTFILES + LINGUAS (meson run_target posync).
set -euo pipefail

src_root="${1:?usage: posync.sh SOURCE_ROOT}"
cd "$src_root/po"

poflags=(--no-wrap)
xgettext "${poflags[@]}" --from-code=UTF-8 \
  --keyword=_ --keyword=N_ --keyword=OPTION:3 --keyword=OPTARG:3 \
  --language=C --directory=.. --output=bas_c.pot --files-from=POTFILES

while IFS= read -r lang; do
  [ -n "$lang" ] || continue
  case "$lang" in
    '#'*) continue ;;
  esac
  po_file="$lang.po"
  if [ ! -f "$po_file" ]; then
    msginit "${poflags[@]}" --no-translator --input=bas_c.pot --locale="$lang" --output-file="$po_file"
  fi
  msgmerge "${poflags[@]}" --update --backup=none "$po_file" bas_c.pot
  msgattrib "${poflags[@]}" --no-obsolete --output-file="$po_file" "$po_file"
done < LINGUAS
