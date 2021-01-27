#!/bin/bash

export root_dir=${1#--root=}
export cpplint_parent=../export
export cpplint_dir=$cpplint_parent/scratch/cpplint-$USER/$(basename $PWD)
rm -rf $cpplint_dir
export cpplint_current_dir=$cpplint_dir
mkdir -p $cpplint_current_dir
export files=${@:2}
export cpplint_main_directory=$(dirname "$0")
export confName="CPPLINT.cfg"
export configPath="${cpplint_main_directory%/*}"/$confName
export dirNotChecked=("." ".." "export" "googletest" "cpplint" "bin" "lab01_git_basics" "lab02_environments" "lab03_build_process" "lab04_class_basics" "lab05_dynamic_memory" "lab06_gotcha")
export extensionsChecked=("cc" "h" "cu" "cpp")
processDirectory () {
	local loc_file_dir=$1
	local loc_cpplint_current_dir=$2
	local loc_cpplint_dir=$3
	local loc_cpplint_main_directory=$4
	if [[ -d $loc_file_dir ]]; then	
		local dirName=$(basename "$loc_file_dir")
		cp -rf "$loc_file_dir" "$loc_cpplint_current_dir"
		cp -f "$configPath" "$loc_cpplint_current_dir"
		for f in `echo $loc_cpplint_current_dir/$dirName/*`;
		do
			local temp=$(basename $f)
			if [[ ! " ${dirNotChecked[@]} " =~ " ${temp} " ]]; then
				    # whatever you want to do when array doesn't contain value
				    processDirectory "$loc_file_dir/$temp" "$loc_cpplint_current_dir/$dirName" "$loc_cpplint_dir" "$loc_cpplint_main_directory"
			fi
		done
	elif [[ -f $loc_file_dir ]]; then
		local exten=$(basename "$loc_file_dir")
		local exten="${exten##*.}"
		if [[  " ${extensionsChecked[@]} " =~ " ${exten} " ]]; then
			$loc_cpplint_main_directory/cpplint.py --root=$loc_cpplint_dir $loc_cpplint_current_dir/$(basename "$loc_file_dir")
		fi
	else
		#echo "$loc_file_dir is invalid"
		if [[ ! $(basename "$loc_file_dir") == "$confName" ]]; then
			if [[ ! $(basename "$loc_file_dir") == "$file" ]]; then
				echo "$loc_file_dir contains $(basename "$loc_file_dir") which is invalid"
			fi
		fi
	fi
}

processDirectory "./$root_dir" "$cpplint_current_dir" "$cpplint_dir" "$cpplint_main_directory"
rm -rf $cpplint_parent
