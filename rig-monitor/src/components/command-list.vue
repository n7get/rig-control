<template>
    <div class="my-3">
        <div class="mb-2">Setttings:</div>
        <b-list-group>
            <b-list-group-item class="d-flex justify-content-between align-items-left"
                v-for="rp in commands"
                :key="rp.name"
            >
                <div class="d-flex justify-content-between w-100">
                    <div @click="edit_command(rp)">{{ rp.desc }}</div>
                    <div class="d-flex gap-3">
                        <div>{{ rp.value }}</div>
                        <icon-trash @click="remove_command(rp.name)" />
                    </div>
                </div>
            </b-list-group-item>
        </b-list-group>
        <button class="btn btn-secondary mt-2 w-100" @click="open_modal = true">Add Setting</button>
    </div>

    <b-modal
        @ok="close_modal"
        @esc="close_modal"
        @close="close_modal"
        v-model="open_modal"
        title="Add Command"
        ok-only
        ok-title="Close"
    >
        <property-list
            title="All Settings"
            :list="all_settings"
            :open="all_settings_open"
            @header-click="toggle_all_settings"
            @property-click="select_property"
        />
        <property-list
            title="All Menu Items"
            :list="all_menu_items"
            :open="all_menu_items_open"
            @header-click="toggle_all_menu_items"
            @property-click="select_property"
        />
    </b-modal>
</template>

<script setup>
import { computed, onMounted, ref } from 'vue';
import iconTrash from '@/components/icons/trash.vue';
import { useRouter } from 'vue-router';
import { useGlobalStore } from '@/stores/global';
import { rig_property } from '@/js/rig_property.js';
import { get_all_settings_list, get_all_menus_list } from '@/js/rig_property.js';

const props = defineProps({
    commands: {
        type: Array,
        required: true,
    },
});

const router = useRouter();
const emit = defineEmits(['add-command', 'remove-command', 'update-command']);

const commands = computed(() => {
    return props.commands.map(cmd => rig_property(cmd.name, cmd.value));
});

onMounted(() => {
    all_settings.value = get_all_settings_list();
    all_menu_items.value = get_all_menus_list();
});

function edit_command(rp) {
    useGlobalStore().openModal(rp, (value) => {
        emit('update-command', { name: rp.name, value: value });
    });
}

function back() {
    router.back();
}

const all_settings = ref([]);
const all_menu_items = ref([]);

function remove_command(name) {
    emit('remove-command', name);
}
const select_property = (rp_to_add) => {
    emit('add-command', {name: rp_to_add.name, value: ref(rp_to_add.value)});
    close_modal();
};

let all_settings_open = ref(true);
const toggle_all_settings = () => {
    all_settings_open.value = !all_settings_open.value;
};

let all_menu_items_open = ref(true);
const toggle_all_menu_items = () => {
    all_menu_items_open.value = !all_menu_items_open.value;
};

const open_modal = ref(false);
function close_modal() {
    open_modal.value = false;
}
</script>