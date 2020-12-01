;Compile with Z88DK with the command:
;z80asm -b pruebazxunopixel.asm
;
; 
;
;
		org 49152


enable_prism_mode:
        ld bc,64571
        ld a,80
        out (c),a
        inc b
        ld a,128
        out (c),a
        ret

disable_prism_mode:
        ld bc,64571
        ld a,80
        out (c),a
        inc b
        xor a
        out (c),a
        ret        

set_rom:
        ;bit bajo
        push af
        rlca
        rlca
        rlca
        rlca
        and 16
        ld bc,32765
        out (c),a
        pop af

        ;bit alto
        rlca
        and 4
        ld bc,8189
        out (c),a
        ret

	
	;z80_byte rom_entra=((puerto_32765>>4)&1) + ((puerto_8189>>1)&2);

	;z80_byte rom1f=(puerto_8189>>1)&2;
	;z80_byte rom7f=(puerto_32765>>4)&1;
	
;poke function to desired vram
;set address in 23296
;set vram in 23298
;set value in 23299

;c02f = 49199
poke_vram:
		di

        ld hl,(23296)
        ld a,(23299)
        push hl
        push af

		call enable_prism_mode

        ld a,(23298)
        call set_rom

        pop af
        pop hl

        ld (hl),a

        call disable_prism_mode

        ld a,3
        call set_rom

        ei

        ret
;c04e = 49230
view_prism:
        di
        call enable_prism_mode

        call wait_no_key

        call wait_key

        call disable_prism_mode

        ei

        ret



read_all_keys:
        xor a
        in a,(254)
        and 31
        ret

wait_no_key:
                call read_all_keys
                cp 31
                jr nz,wait_no_key
                ret

wait_key:
                call read_all_keys
                cp 31
                jr z,wait_key
                ret      
